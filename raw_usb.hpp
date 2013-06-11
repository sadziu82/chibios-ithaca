#ifndef USBDESCRIPTOR_H_INCLUDED
#define USBDESCRIPTOR_H_INCLUDED

#include "usb.h"
#include "string.h"

/*
 * This file contains the USB descriptors. For Details see
 *   http://www.beyondlogic.org/usbnutshell/usb5.shtml
 *   http://www.usbmadesimple.co.uk/ums_4.htm
 */

/*
 * @brief   raw usb constants: size of usb packets and buffer depth
 */
static const uint8_t RAW_USB_EP_IN = 1;
static const uint8_t RAW_USB_EP_OUT = 2;
static const uint8_t RAW_USB_PACKET_SIZE = 64;
static const uint8_t RAW_USB_BUFFER_DEPTH = 8;

/*
 * USB Device Descriptor.
 */
static const uint8_t usb_device_descriptor_data[ 18 ] = {
    USB_DESC_DEVICE(
        0x0200,        /* bcdUSB USB version */
        0xFF,          /* bDeviceClass (FF=Vendor specific */
        0x00,          /* bDeviceSubClass.                 */
        0x00,          /* bDeviceProtocol.                 */
        0x40,          /* bMaxPacketSize0.                 */
        0x0483,        /* idVendor (ST).                   */
        0xFFFF,        /* idProduct.                       */
        0x0200,        /* bcdDevice Device Release Number  */
        0x01,          /* Index of Manufacturer String Descriptor     */
        0x02,          /* iProduct.                        */
        0x03,          /* iSerialNumber.                   */
        0x01           /* bNumConfigurations.              */
)};

/*
 * Device Descriptor wrapper.
 */
static const USBDescriptor usb_device_descriptor = {
    sizeof( usb_device_descriptor_data ),
    usb_device_descriptor_data
};

/*
 * Configuration Descriptor
 */
//MaxPacketsize for Bulk Full-Speed is 0x40

/*
 *
 */
static const uint8_t usb_config_descriptor_data[ 9 + 9 + 7 + 7 ] = {
    // USB config #1, 9 bytes
    USB_DESC_CONFIGURATION(
        sizeof( usb_config_descriptor_data ),           // wTotalLength
        0x01,                                           // bNumInterfaces
        0x01,                                           // bConfigurationValue
        0x00,                                           // iConfiguration
        0x80,                                           // self pwered
        0xFA                                            // 500mA
    ),
    // interface descriptor, 9 bytes
    USB_DESC_INTERFACE(
        0x00,          /* bInterfaceNumber.                */
        0x00,          /* bAlternateSetting.               */
        0x02,          /* bNumEndpoints.                   */
        0xFF,          /* bInterfaceClass (VendorSpecific) */
        0x00,          /* bInterfaceSubClass               */
        0x00,          /* bInterfaceProtocol               */
        0x00           /* iInterface.                      */
    ),
    /* EndOBpoint 1 Descriptor, Direction in*/ //7 Bytes
    USB_DESC_ENDPOINT(
        RAW_USB_EP_IN | USB_RTYPE_DIR_DEV2HOST,  /* bEndpointAddress */
        0x03,                      /* bmAttributes (Bulk)  */
        RAW_USB_PACKET_SIZE,             /* wMaxPacketSize       */
        5                     /* bInterval            */
    ),
    // Endpoint 2 Descriptor Direction out //7 Bytes
    USB_DESC_ENDPOINT(
        RAW_USB_EP_OUT | USB_RTYPE_DIR_HOST2DEV,  // bEndpointAddress
        0x03,                      // bmAttributes (interrupt)
        RAW_USB_PACKET_SIZE,            // wMaxPacketSize
        5                       // bInterval
    )
};

/*
 * Configuration Descriptor wrapper.
 */
static const USBDescriptor usb_config_descriptor = {
    sizeof( usb_config_descriptor_data ),
    usb_config_descriptor_data
};

/*
 * U.S. English language identifier.
 */
static const uint8_t stringLanguage[] = {
    USB_DESC_BYTE( 4 ),                         // bLength
    USB_DESC_BYTE( USB_DESCRIPTOR_STRING ),     // bDescriptorType
    USB_DESC_WORD( 0x0409 )                     // wLANGID (U.S. English)
};

/*
 * Vendor string.
 */
static const uint8_t stringVendor[] = {
    USB_DESC_BYTE( 20 ),                            // bLength
    USB_DESC_BYTE( USB_DESCRIPTOR_STRING ),         // bDescriptorType
    'i', 0, 't', 0, 'h', 0, 'a', 0, 'c', 0, 'a', 0,
    '.', 0, 'p', 0, 'l', 0
};

/*
 * Device Description string.
 */
static const uint8_t stringDeviceDescriptor[] = {
    USB_DESC_BYTE( 20 ),                            // bLength
    USB_DESC_BYTE( USB_DESCRIPTOR_STRING ),         // bDescriptorType
    'R', 0, 'C', 0, ' ', 0, 'R', 0, 'e', 0, 'm', 0,
    'o', 0, 't', 0, 'e', 0
};

/*
 * Serial Number string.
 */
static const uint8_t stringSerialNumber[] = {
  USB_DESC_BYTE( 10 ),                              // bLength
  USB_DESC_BYTE( USB_DESCRIPTOR_STRING ),           // bDescriptorType
  '4', 0, 'C', 0, 'A', 0, '7', 0
};

/*
 * String not found string.
 */
static const uint8_t stringNotFound[] = {
  USB_DESC_BYTE(34),                    /* bLength .                        */
  USB_DESC_BYTE(USB_DESCRIPTOR_STRING), /* bDescriptorType.                 */
  'S', 0, 't', 0, 'r', 0, 'i', 0, 'n', 0, 'g', 0, ' ', 0, 'n', 0,
  'o', 0, 't', 0, ' ', 0, 'f', 0, 'o', 0, 'u', 0, 'n', 0, 'd', 0
};

/*
 * Strings wrappers array.
 */
static const USBDescriptor usb_strings[] = {
  {sizeof stringLanguage,           stringLanguage},
  {sizeof stringVendor,             stringVendor},
  {sizeof stringDeviceDescriptor,   stringDeviceDescriptor},
  {sizeof stringSerialNumber,       stringSerialNumber},
  {sizeof stringNotFound,           stringNotFound}
};

/*
 *  @brief  raw usb class
 */
class RawUSB {
private:
    //
    static USBDriver *driver;
    static bool ready_flag;
    static const USBConfig usb_iface_config;
    // data read from host, out ep
    static uint8_t out_buffer[ RAW_USB_PACKET_SIZE ];
    static uint8_t out_buffer_queue[ RAW_USB_BUFFER_DEPTH ][ RAW_USB_PACKET_SIZE ];
    // data send to host, in ep
    static uint8_t in_buffer[ RAW_USB_PACKET_SIZE ];
    static uint8_t in_buffer_queue[ RAW_USB_BUFFER_DEPTH ][ RAW_USB_PACKET_SIZE ];
    static uint32_t in_buffer_w, in_buffer_r;
    //
    static void ep_in_cb( USBDriver *usbp, usbep_t ep );
    static USBInEndpointState ep_in_state;
    static const USBEndpointConfig ep_in_config;
    //
    static void ep_out_cb( USBDriver *usbp, usbep_t ep );
    static USBOutEndpointState ep_out_state;
    static const USBEndpointConfig ep_out_config;
    // 
    static void usb_event( USBDriver *usbp, usbevent_t event );
    static bool_t requests_hook( USBDriver *usbp );
    // 
    static msg_t worker_thread( void *arg );
public:
    //
    static void init( void );
    static bool read( uint8_t * );
    static bool write( const uint8_t * );
    static bool write( const char * );
};

#endif // USBDESCRIPTOR_H_INCLUDED

