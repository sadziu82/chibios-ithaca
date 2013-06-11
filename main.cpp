/*
 * @brief   mcu48 ...
 */

/*
 * @brief   mcu48 ...
 */
#include "main.hpp"

/*
 * @brief   radio thread
 */
static WORKING_AREA( waRadioThread, 256 );
static msg_t RadioThread( void *arg ) {
    // 
    ( void )arg;
    radio_packet_t rf_packet;
    systime_t wakeup_time = 0;
    char buf[ 0x40 ];
    // 
    chRegSetThreadName( "radio" );
    //
    while ( TRUE ) {
        // 
        while ( radioInit() == FALSE ) {
            // 
            RawUSB::write( "radio nie dziala" );
            // 
            chThdSleepMilliseconds( 100 );
        }
        //
        RawUSB::write( "radio gotowe" );
        //
        while ( true ) {
            wakeup_time = chTimeNow();
            if ( true ) {
                rf_packet.src = 0xAA;
                rf_packet.dst = 0xFF;
                if ( radioSend( &rf_packet ) == true ) {
                    chsprintf( buf, "pakiet wysłany: (%2X) -> (%2X): %2X", rf_packet.src, rf_packet.dst, rf_packet.data[ 0 ] );
                    RawUSB::write( buf );
                    if ( radioRecv( &rf_packet ) == true ) {
                        chsprintf( buf, "pakiet odebrany: (%2X) -> (%2X): %2X", rf_packet.src, rf_packet.dst, rf_packet.data[ 0 ] );
                        RawUSB::write( buf );
                    }
                }
                wakeup_time += MS2ST( 20 );
                if ( wakeup_time > chTimeNow() ) {
                    chThdSleepUntil( wakeup_time );
                }
            } else {
                if ( radioRecv( &rf_packet ) == true ) {
                    RawUSB::write( "pakiet odebrany" );
                    chThdSleepMilliseconds( 3 );
                    rf_packet.src = 0x55;
                    rf_packet.dst = 0xAA;
                    rf_packet.data[ 0 ]++;
                    if ( radioSend( &rf_packet ) == true ) {
                        RawUSB::write( "pakiet wysłany" );
                    }
                }
            }
        }
        //
        RawUSB::write( "radio zepsute" );
        // 
        chThdSleepMilliseconds( 250 );
    }
    //
    return 0;
}

// 
static const EXTConfig ext_cfg = {
    .channels = {
        { EXT_CH_MODE_DISABLED, NULL },
        { EXT_CH_MODE_DISABLED, NULL },
        { EXT_CH_MODE_DISABLED, NULL },
        { EXT_CH_MODE_DISABLED, NULL },
        { EXT_CH_MODE_DISABLED, NULL },
        { EXT_CH_MODE_DISABLED, NULL },
        { EXT_CH_MODE_DISABLED, NULL },
        { EXT_CH_MODE_DISABLED, NULL },
        { EXT_CH_MODE_DISABLED, NULL },
        { EXT_CH_MODE_DISABLED, NULL },
        { EXT_CH_MODE_DISABLED, NULL },
        { EXT_CH_MODE_DISABLED, NULL },
        { EXT_CH_MODE_DISABLED, NULL },
        { EXT_CH_MODE_FALLING_EDGE | EXT_MODE_GPIOC, si4421IRQ },
        { EXT_CH_MODE_DISABLED, NULL },
        { EXT_CH_MODE_DISABLED, NULL },
    }
};

/*
 * @brief   SPI Configuration for LCD, Touch, SD
 */
const SPIConfig ui_spi_cfg = {
    NULL,
    GPIOB, 12,                   // not used
    SPI_CR1_MSTR,
};

void LcdST7735R::paint_screen( bool key_frame = false ) {
    //
    static uint16_t x, i;
    static systime_t time = 0, last_time = 0, delta = 0;
    static systime_t fps_time = 0, fps_last_time = 0, fps = 0;
    static char buffer[ 0x20 ] = { 0 };
    delta = last_time - time;
    time = chTimeNow();
    if ( key_frame ) {
        if ( fps_last_time != 0 ) {
            fps = fps_time - fps_last_time;
        }
        fps_last_time = fps_time;
        fps_time = chTimeNow();
        if ( x++ > this->lcd_height ) {
            x = 0;
        }
    }
    //// //
    //// memset( buffer, 0, 32 );
    //// chsprintf( buffer, "t2-t1: %d", this->time_end - this->time_start );
    //// this->draw_string( 0, 0, buffer, Lcd::Color::Green );
    //// memset( buffer, 0, 32 );
    //// chsprintf( buffer, "delta: %d", delta );
    //// this->draw_string( 0, 8, buffer, Lcd::Color::LightBlue );
    memset( buffer, 0, 32 );
    chsprintf( buffer, "%f / %0.1f", 1000.0 / fps, chTimeNow() / 1000.0 );
    this->draw_string( 0, 120, buffer, Lcd::Color::Red );
    // 
    last_time = chTimeNow();
}

void paint_rfm12b_status( Lcd *lcd, bool key_frame = false ) {
    static uint16_t alarm = 0;
    LcdColor fg, bg;
    alarm ++;
    lcd->draw_string( 80, 1, " RC Remote ", Lcd::Color::White, Lcd::Color::Blue, Lcd::Align::Center );
    if ( alarm > 900 ) { 
        alarm = 0;
    } else if ( alarm > 600 ) { 
        fg = Lcd::Color::Red;
    } else if ( alarm > 300 ) { 
        fg = Lcd::Color::Yellow;
    } else {
        fg = Lcd::Color::White;
    }
    lcd->draw_string( 1, 15, "11.34 [V]", fg );
    lcd->draw_string( 1, 25, "12.67 [A]", Lcd::Color::White );
    lcd->draw_string( 1, 35, "13:45 [s]", Lcd::Color::White );
    lcd->draw_string( 1, 45, "248 [m]", Lcd::Color::White );
    lcd->draw_string( 159, 15, "17'34\"34 N", Lcd::Color::White, Lcd::Color::Transparent, Lcd::Align::Right );
    lcd->draw_string( 159, 25, "51'19\"22 E", Lcd::Color::White, Lcd::Color::Transparent, Lcd::Align::Right );
    lcd->draw_string( 159, 35, "123 m n.p.m.", Lcd::Color::White, Lcd::Color::Transparent, Lcd::Align::Right );
    lcd->draw_string( 159, 45, "12 km/h", Lcd::Color::White, Lcd::Color::Transparent, Lcd::Align::Right );
    lcd->draw_string( 1, 60, "2 [%]", fg );
}

/*
 * @brief   lcd thread
 */
static WORKING_AREA( waUiThread, 256 );
static msg_t UiThread( void *arg ) {
    // 
    ( void )arg;
    chRegSetThreadName( "ui" );
    //
    static LcdST7735R lcd;
    static StdFont font;
    systime_t wakeup_time = 0;
    // enable spi
    spiStart( &SPID2, &ui_spi_cfg );
    // lcd: spi2, ce, dc, rst
    lcd.init( &SPID2, GPIOB, 12, GPIOA, 10, GPIOA, 9 );
    lcd.set_font( &font );
    lcd.set_bg_color( Lcd::Color::Black );
    lcd.add_paint_function( paint_rfm12b_status );
    //lcd.set_rotation( Lcd::Rotation::Normal );
    //lcd.clear_screen( 0xFFFF );
    // loop
    wakeup_time = chTimeNow();
    while ( TRUE ) {
        //lcd.set_bg_color( color++ );
        lcd.update_lcd();
        wakeup_time += MS2ST( 100 );
        chThdSleepUntil( wakeup_time );
        //chThdSleepMilliseconds( 100 );
    }
    //
    return 0;
}

// entry point
int main( void ) {
    // 
    systime_t wakeup_time = 0;
    // 
    halInit();
    chSysInit();
    //
    RawUSB::init();
    //
    extStart( &EXTD1, &ext_cfg );
    // 
    chThdCreateStatic( waRadioThread, sizeof( waRadioThread ), NORMALPRIO + 10, RadioThread, NULL );
    // 
    chThdCreateStatic( waUiThread, sizeof( waUiThread ), NORMALPRIO, UiThread, NULL );
    // 
    wakeup_time = chTimeNow();
    while ( TRUE ) {
        // toggle led
        //palTogglePad( SYS_LED_PORT, SYS_LED_PIN );
        //
        wakeup_time += MS2ST( 500 );
        chThdSleepUntil( wakeup_time );
    }
}

