/*
 * @brief   mcu48 ...
 */


/*
 * @brief   mcu48 ...
 */
#include "ch.h"
#include "hal.h"

/*
 * @brief   mcu48 ...
 */
#include "raw_usb.hpp"


// usb driver to use
USBDriver *RawUSB::driver;
bool RawUSB::ready_flag;

// function to read data from host (out ep)
bool RawUSB::read( uint8_t *data ) {
    return true;
};

// function to write data to host (in ep)
bool RawUSB::write( const uint8_t *data ) {
    return true;
};

// 
uint32_t RawUSB::in_buffer_w;
uint32_t RawUSB::in_buffer_r;

// function to write data to host (in ep)
bool RawUSB::write( const char *data ) {
    uint8_t w = RawUSB::in_buffer_w % RAW_USB_BUFFER_DEPTH;
    //
    if ( RawUSB::in_buffer_w - RawUSB::in_buffer_r >= RAW_USB_BUFFER_DEPTH ) {
        RawUSB::in_buffer_r++;
    }
    // write proper buffer
    memcpy( RawUSB::in_buffer_queue[ w ], data, RAW_USB_PACKET_SIZE );
    // increase write pointer
    RawUSB::in_buffer_w++;
    return true;
};

//
uint8_t RawUSB::out_buffer[ RAW_USB_PACKET_SIZE ];
uint8_t RawUSB::out_buffer_queue[ RAW_USB_BUFFER_DEPTH ][ RAW_USB_PACKET_SIZE ];
// 
uint8_t RawUSB::in_buffer[ RAW_USB_PACKET_SIZE ];
uint8_t RawUSB::in_buffer_queue[ RAW_USB_BUFFER_DEPTH ][ RAW_USB_PACKET_SIZE ];

/*
 * data Transmitted Callback
 */
void RawUSB::ep_in_cb( USBDriver *usbp, usbep_t ep ){
    uint8_t r = RawUSB::in_buffer_r % RAW_USB_BUFFER_DEPTH;
    (void) usbp;
    (void) ep;
    // exit on USB reset
    if ( !RawUSB::ready_flag ) return;
    if ( RawUSB::in_buffer_r != RawUSB::in_buffer_w ) {
        memcpy( RawUSB::in_buffer,
                RawUSB::in_buffer_queue[ r ],
                RAW_USB_PACKET_SIZE );
        RawUSB::in_buffer_r++;
        
    } else {
        memset( RawUSB::in_buffer, 0, RAW_USB_PACKET_SIZE );
    }
    // copy buffer
    /////// if ( RawUSB::ep_in_prebuffer_empty == FALSE ) {
    ///////     memcpy( RawUSB::in_buffer, RawUSB::ep_in_prebuffer, RAW_USB_PACKET_SIZE );
    ///////     RawUSB::ep_in_prebuffer_empty = TRUE;
    /////// } else {
    ///////     memset( RawUSB::in_buffer, 0, RAW_USB_PACKET_SIZE );
    /////// }
    // and go 
    usbPrepareTransmit( usbp, RAW_USB_EP_IN, RawUSB::in_buffer, RAW_USB_PACKET_SIZE );
    chSysLockFromIsr();
    usbStartTransmitI( usbp, RAW_USB_EP_IN );
    chSysUnlockFromIsr();

}

/**
 * @brief   IN EP1 state.
 */
USBInEndpointState RawUSB::ep_in_state;

/**
 * @brief   EP1 initialization structure (IN only).
 */
const USBEndpointConfig RawUSB::ep_in_config = {
    USB_EP_MODE_TYPE_INTR,    //Type and mode of the endpoint
    NULL,                     //Setup packet notification callback (NULL for non-control EPs)
    RawUSB::ep_in_cb,          //IN endpoint notification callback
    NULL,                     //OUT endpoint notification callback
    RAW_USB_PACKET_SIZE,    //IN endpoint maximum packet size
    0x0000,                   //OUT endpoint maximum packet size
    &RawUSB::ep_in_state, //USBEndpointState associated to the IN endpoint
    NULL,                     //USBEndpointState associated to the OUTendpoint
    1,
    NULL                      //Pointer to a buffer for setup packets (NULL for non-control EPs)
};

/*
 * data Received Callback
 * It toggles an LED based on the first received character.
 */
void RawUSB::ep_out_cb( USBDriver *usbp, usbep_t ep ) {
    // USBOutEndpointState *osp = usbp->epc[ep]->out_state;
    (void) usbp;
    (void) ep;
    // exit on USB reset
    if ( !RawUSB::ready_flag ) return;
    // read buffer to temporary space
    /////////// if ( RawUSB::ep_out_prebuffer_ready == FALSE ) {
    ///////////     memcpy( RawUSB::ep_out_prebuffer, RawUSB::out_buffer, RAW_USB_PACKET_SIZE );
    ///////////     RawUSB::ep_out_prebuffer_ready = TRUE;
    /////////// }
    // prepare for the next transfer
    usbPrepareReceive( usbp, RAW_USB_EP_OUT, RawUSB::out_buffer, RAW_USB_PACKET_SIZE );
    // 
    chSysLockFromIsr();
    usbStartReceiveI( usbp, RAW_USB_EP_OUT );
    chSysUnlockFromIsr();
}

/**
 * @brief   OUT EP2 state.
 */
USBOutEndpointState RawUSB::ep_out_state;

/**
 * @brief   EP2 initialization structure (OUT only).
 */
const USBEndpointConfig RawUSB::ep_out_config = {
    USB_EP_MODE_TYPE_BULK,
    NULL,
    NULL,
    RawUSB::ep_out_cb,
    0x0000,
    RAW_USB_PACKET_SIZE,
    NULL,
    &RawUSB::ep_out_state,
    1,
    NULL
};


/*
 * Handles the USB driver global events.
 */
void RawUSB::usb_event( USBDriver *usbp, usbevent_t event ) {
    (void) usbp;
    switch (event) {
        case USB_EVENT_RESET:
            // mark usb as inactive
            RawUSB::ready_flag = FALSE;
            return;
        case USB_EVENT_ADDRESS:
            return;
        case USB_EVENT_CONFIGURED:
            // configure usb endpoints
            chSysLockFromIsr();
            usbInitEndpointI( usbp, RAW_USB_EP_IN, &RawUSB::ep_in_config );
            usbInitEndpointI( usbp, RAW_USB_EP_OUT, &RawUSB::ep_out_config );
            chSysUnlockFromIsr();
            // prepare endpoint for transfer
            usbPrepareTransmit( usbp, RAW_USB_EP_IN, RawUSB::in_buffer, RAW_USB_PACKET_SIZE );
            chSysLockFromIsr();
            usbStartTransmitI( usbp, RAW_USB_EP_IN );
            chSysUnlockFromIsr();
            // prepare endpoint for transfer
            usbPrepareReceive( usbp, RAW_USB_EP_OUT, RawUSB::out_buffer, RAW_USB_PACKET_SIZE );
            chSysLockFromIsr();
            usbStartReceiveI( usbp, RAW_USB_EP_OUT );
            chSysUnlockFromIsr();
            //allow the main thread to init the transfers
            RawUSB::ready_flag = TRUE;
            return;
        case USB_EVENT_SUSPEND:
            // mark usb as inactive
            RawUSB::ready_flag = FALSE;
            return;
        case USB_EVENT_WAKEUP:
            return;
        case USB_EVENT_STALLED:
            return;
    }
    return;
}

/*
 * Returns the USB descriptors defined in usbdescriptor.h
 */
static const USBDescriptor *get_descriptor( USBDriver *usbp,
            uint8_t dtype, uint8_t dindex, uint16_t lang ) {
    //
    (void)usbp;
    (void)lang;
    // 
    switch ( dtype ) {
        case USB_DESCRIPTOR_DEVICE:
            return &usb_device_descriptor;
        case USB_DESCRIPTOR_CONFIGURATION:
            return &usb_config_descriptor;
        case USB_DESCRIPTOR_STRING:
            if ( dindex < 4 ){
                return &usb_strings[ dindex ];
            } else {
                return &usb_strings[ 4 ];
            }
    }
    return NULL;
}

/*
 * Requests hook callback.
 * This hook allows to be notified of standard requests or to
 *          handle non standard requests.
 * This implementation does nothing and passes all requests to
 *     the upper layers
 */
bool_t RawUSB::requests_hook( USBDriver *usbp ) {
    (void) usbp;
    return RawUSB::ready_flag;
}

/*
 * USBconfig
 */
const USBConfig RawUSB::usb_iface_config = {
    RawUSB::usb_event,
    get_descriptor,
    RawUSB::requests_hook,
    NULL
};

/*
 * @brief  usb interface thread
 */
static WORKING_AREA( waRawUSB, 128 );
msg_t RawUSB::worker_thread( void *arg ) {
    // 
    ( void )arg;
    uint8_t i;
    // 
    chRegSetThreadName( "usb iface" );
    // disconect from usb bus
    usbDisconnectBus( usb_driver );
    chThdSleepMilliseconds( 50 );
    // connect usb bus
    usbStart( RawUSB::driver, &usb_iface_config );
    usbConnectBus( RawUSB::driver );
    // 
    while ( TRUE ) {
        // wait for usb to become configured
        chThdSleepMilliseconds( 50 );
    }
    return 0;
}

//
void RawUSB::init( void ) {
    // usb driver to use
    RawUSB::driver = &USBD1;
    RawUSB::ready_flag = false;
    //
    RawUSB::in_buffer_w = 0;
    RawUSB::in_buffer_r = 0;
    // 
    chThdCreateStatic( waRawUSB, sizeof( waRawUSB ), NORMALPRIO,
                       RawUSB::worker_thread, NULL );
}

