/*
 * @brief   Radio library
 */

#include "radio.hpp"

/*
 * @brief   si4421 GENCREG bits
 */
const uint16_t SI4421_GENCREG = 0x8000;
const uint16_t SI4421_TXDEN = 0x80;
const uint16_t SI4421_RXFIFOEN = 0x40;
const uint16_t SI4421_FBS_868 = 0x20;
const uint16_t SI4421_LCS_12p5 = 0x08;
// GENCREG: 868 MHz, 12.5 pF
const uint16_t SI4421_GENCREG_IDLE = SI4421_GENCREG | SI4421_FBS_868 |
                                     SI4421_LCS_12p5;

/*
 * @brief   si4421 PMCREG bits
 */
const uint16_t SI4421_PMCREG = 0x8200;
const uint16_t SI4421_RXCEN = 0x80;
const uint16_t SI4421_BBCEN = 0x40;
const uint16_t SI4421_TXCEN = 0x20;
const uint16_t SI4421_SYNEN = 0x10;
const uint16_t SI4421_OSCEN = 0x08;
const uint16_t SI4421_LBDEN = 0x04;
const uint16_t SI4421_WUTEN = 0x02;         // 
const uint16_t SI4421_CLKOEN = 0x01;        // 1 - disable
// PMCREG idle mode: oscillator, synthesier and baseband on
const uint16_t SI4421_PMCREG_IDLE = SI4421_PMCREG | SI4421_CLKOEN |
                                    SI4421_OSCEN | SI4421_SYNEN |
                                    SI4421_BBCEN;

/*
 * @brief   si4421 PMCREG bits
 */
const uint16_t SI4421_FIFORSTREG = 0xCA00;
const uint16_t SI4421_FFBC_8 = 0x80;
const uint16_t SI4421_FSCF = 0x02;
// PMCREG idle mode: oscillator, synthesier and baseband on
const uint16_t SI4421_FIFORSTREG_IDLE = SI4421_FIFORSTREG |
                                        SI4421_FFBC_8;

// TODO
char tmp_buff[ 64 ];

/*
 * @brief   Radio state information
 */
radio_state_t radio_state = RADIO_STATE_UNINIT;

/*
 * @brief   Radio statistics
 */
radio_stats_t radio_stats;

/*
 * @brief   Radio ...
 */
Semaphore radio_semaphore;


// rfm12b xfer
uint16_t si4421Xfer( const uint16_t cmd ) {
    static uint16_t data = 0;
    spiSelect( &SI4421_SPI );
    spiExchange( &SI4421_SPI, 1, &cmd, &data );
    spiUnselect( &SI4421_SPI );
    return data;
}

// 
const SPIConfig si4421_spi_cfg = {
    NULL,
    SI4421_CE_PORT,
    SI4421_CE_PIN,
    SPI_CR1_DFF | SPI_CR1_MSTR | SPI_CR1_BR_1,
};

//
void si4421ResetFifo( void ) {
    si4421Xfer( SI4421_FIFORSTREG_IDLE );
    si4421Xfer( SI4421_FIFORSTREG_IDLE | SI4421_FSCF );
}

/*
 * @brief   enable tx data register
 */
void si4421TxEn( void ) {
    si4421Xfer( SI4421_GENCREG_IDLE | SI4421_TXDEN );
}

/*
 * @brief   enable rx fifo usage
 */
void si4421RxEn( void ) {
    si4421Xfer( SI4421_GENCREG_IDLE | SI4421_RXFIFOEN );
}

/*
 * @brief   turn off both tx and rx chains
 */
void si4421RadioIdle( void ) {
    si4421Xfer( SI4421_PMCREG_IDLE );
    si4421Xfer( SI4421_GENCREG_IDLE );
}

/*
 * @brief   turn on tx chain
 */
void si4421TxMode( void ) {
    si4421Xfer( SI4421_PMCREG_IDLE | SI4421_TXCEN );
}

/*
 * @brief   turn on rx chain
 */
void si4421RxMode( void ) {
    si4421Xfer( SI4421_PMCREG_IDLE | SI4421_RXCEN );
}

void si4421EnableSPI( void ) {
    // configure and acquire spi
    spiStart( &SI4421_SPI, &si4421_spi_cfg );
    spiAcquireBus( &SI4421_SPI );
}

void si4421DisableSPI( void ) {
    // release bus
    spiReleaseBus( &SI4421_SPI );
    spiStop( &SI4421_SPI );
}

void si4421EnableIRQ( void ) {
    // enable interrupt from radio
    extChannelEnable( &EXTD1, SI4421_IRQ_PIN );
}

void si4421DisableIRQ( void ) {
    // disable interrupt from radio
    extChannelDisable( &EXTD1, SI4421_IRQ_PIN );
}

msg_t si4421WaitIRQ( uint16_t timeout ) {
    return chSemWaitTimeout( &radio_semaphore, MS2ST( timeout ) );
}

void si4421StartTx( void ) {
    // configure and acquire spi
    si4421EnableSPI();
    // clear any pending interrupt flags
    si4421Xfer( 0x0000 );
    // clear semaphore
    chSemReset( &radio_semaphore, 0 );
    // enable interrupts
    si4421EnableIRQ();
    // enable transmitter & start transmission
    si4421TxEn();
    // 
    si4421TxMode();
}

void si4421StartRx( void ) {
    // configure and acquire spi
    si4421EnableSPI();
    // clear semaphore
    chSemReset( &radio_semaphore, 0 );
    // enable transmitter & start transmission
    si4421RxEn();
    // 
    si4421RxMode();
    //
    si4421ResetFifo();
    // clear any pending interrupt flags
    si4421Xfer( 0x0000 );
    // enable interrupts
    si4421EnableIRQ();
}

void si4421Stop( void ) {
    si4421RadioIdle();
    si4421DisableIRQ();
    si4421DisableSPI();
}

// init is needed after this
void si4421Abort( void ) {
    // normal stop
    si4421Stop();
    // just in case put module into reset
    palClearPad( SI4421_RST_PORT, SI4421_RST_PIN );
}

//
bool_t si4421Init( void ) {
    // 
    uint16_t status = 0, tmout = 0;
    //
    radio_state = RADIO_STATE_UNINIT;
    chSemInit( &radio_semaphore, 0 );
    chSemReset( &radio_semaphore, 0 );
    // enable SPI and interrupts from radio module
    si4421EnableSPI();
    // reset module
    palClearPad( SI4421_RST_PORT, SI4421_RST_PIN );
    chThdSleepMilliseconds( 10 );
    palSetPad( SI4421_RST_PORT, SI4421_RST_PIN );
    // 
    tmout = 0;
    status = 0;
    // 
    while ( palReadPad( SI4421_IRQ_PORT, SI4421_IRQ_PIN ) == 0 ) {
        //
        if ( tmout++ > SI4421_TIMEOUT ) {
            // 
            si4421Abort();
            return FALSE;
        } else {
            status |= si4421Xfer( 0x0000 );
            chThdSleepMilliseconds( 10 );
        }
    }
    // DEBUG
    chsprintf( tmp_buff, "status: %04X", status );
    RawUSB::write( tmp_buff );
    // check if we have a good flag set
    if ( ( status & 0x4000 ) == 0 ) {
            // just in case put module into reset
            palClearPad( SI4421_RST_PORT, SI4421_RST_PIN );
            // 
            si4421Abort();
            return FALSE;
    }
    /* DEBUG */ RawUSB::write( "radio present" );
    // in here radio should be out of PoR
    radio_state = RADIO_STATE_READY;
    // configure si4421 module
    si4421ResetFifo();                  // reset fifo mode
    si4421Xfer( SI4421_GENCREG_IDLE );  // 868, 12pf
    si4421Xfer( 0xC4F7 );               // afc auto
    si4421Xfer( 0xA640 );               // cfs, 868 MHz
    //si4421Xfer( 0xC606 );               // data rate, 49.261 kbps
    si4421Xfer( 0xC605 );               // data rate, 57.471 kbps
    //si4421Xfer( 0xC623 );               // data rate, 57.471 kbps
    si4421Xfer( 0xC2AC );               // data filter 
    si4421Xfer( 0xCC17 );               // pll
    si4421Xfer( 0xC000 );               // lowbat disabled
    si4421Xfer( SI4421_PMCREG_IDLE );   // osc output disabled
    si4421Xfer( 0x9083 );               // rx config
    si4421Xfer( 0x9850 );               // tx config
    si4421Xfer( 0xCED4 );               // synchro byte
    // antenna tuning
    si4421TxMode();                 // tx on, rx off, antenna tuning
    chThdSleepMilliseconds( 5 );
    si4421RadioIdle();              // antenna tuned, radio idle
    // 
    si4421Stop();
    return TRUE;
}

// 
void si4421IRQ( EXTDriver *extp, expchannel_t channel ) {
    ( void )extp;
    ( void )channel;
    // lock, release semaphore, unlock
    chSysLockFromIsr();
    //if ( chSemGetCounterI( &radio_semaphore ) < 0 ) {
        chSemSignalI( &radio_semaphore );
    //}
    chSysUnlockFromIsr()
}

//
bool si4421Send( const radio_packet_t *rf_packet ) {
    //
    static uint8_t i, buff[ 0x28 ] = {
        0xAA, 0xAA, 0xAA, 0xAA, 0x2D, 0xD4,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0xAA, 0xAA
    };
    uint8_t tmout;
    memcpy( &buff[ 6 ], rf_packet, 32 );
    // DBEUG
    palTogglePad( SYS_LED_PORT, SYS_LED_PIN );
    //
    tmout = 5;
    si4421StartTx();
    for ( i = 0; i < 0x28; i++ ) {
        if ( si4421WaitIRQ( tmout ) != RDY_OK ) {
            // DEBUG
            palTogglePad( SYS_LED_PORT, SYS_LED_PIN );
            // abort tx
            si4421Abort();
            return false;
        }
        // TODO add checking of irq flags
        // write data
        si4421Xfer( 0xB800 | buff[ i ] );
        tmout = 2;
    }
    // DEBUG
    palTogglePad( SYS_LED_PORT, SYS_LED_PIN );
    // stop tx
    si4421Stop();
    return true;
}

//
bool_t si4421Recv( radio_packet_t *rf_packet ) {
    //
    static uint8_t i, tmout, buff[ 0x20 ];
    //
    tmout = 10;
    si4421StartRx();
    //
    for ( i = 0; i < 0x20; i++ ) {
        //
        if ( chSemWaitTimeout( &radio_semaphore, MS2ST( tmout ) ) != RDY_OK ) {
            si4421Stop();
            //chsprintf( tmp_buff, "data recv failed: %d", i );
            //RawUSB::write( tmp_buff );
            // relese spi bus and return
            palSetPad( SYS_LED_PORT, SYS_LED_PIN );
            return false;
        }
        palClearPad( SYS_LED_PORT, SYS_LED_PIN );
        buff[ i ] = ( si4421Xfer( 0xB000 ) & 0xFF );
        tmout = 2;
    }
    memcpy( rf_packet, buff, 0x20 );
    palSetPad( SYS_LED_PORT, SYS_LED_PIN );
    // 
    si4421Stop();
    return true;
}

//
bool_t radioInit( void ) {
    // 
    return si4421Init();
}

//
bool radioSend( const radio_packet_t *packet ) {
    // 
    return si4421Send( packet );
}

//
bool_t radioRecv( radio_packet_t *packet ) {
    // 
    return si4421Recv( packet );
}

