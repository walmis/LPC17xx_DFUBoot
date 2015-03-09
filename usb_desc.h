#pragma once

#include <usbapi.h>
#include "config.h"

#define MAX_PACKET_SIZE	64

#define LE_WORD(x)		((x)&0xFF),((x)>>8)

// CDC definitions
#define CS_INTERFACE			0x24
#define CS_ENDPOINT				0x25

#define INT_IN_EP		0x81
#define BULK_OUT_EP		0x05

#define BULK_IN_EP		0x82
#define BULK_IN_EP_CDC		0x88
#define USB_MAX_PACKET0 64
#define USB_DFU_XFER_SIZE 256

#include <mw_usbd_desc.h>
#include <mw_usbd_dfu.h>

static const U8 abDescriptors[] = {
// device descriptor
		  USB_DEVICE_DESC_SIZE,              /* bLength */
		  USB_DEVICE_DESCRIPTOR_TYPE,        /* bDescriptorType */
		  WBVAL(0x0200), /* 2.00 */          /* bcdUSB */
		  0x00,                              /* bDeviceClass */
		  0x00,                              /* bDeviceSubClass */
		  0x00,                              /* bDeviceProtocol */
		  USB_MAX_PACKET0,                   /* bMaxPacketSize0 */
		  WBVAL(IDVENDOR),                     /* idVendor */
		  WBVAL(IDPRODUCT),                     /* idProduct */
		  WBVAL(0x0100), /* 1.00 */          /* bcdDevice */
		  0x01,                              /* iManufacturer */
		  0x02,                              /* iProduct */
		  0x03,                              /* iSerialNumber */
		  0x01,                               /* bNumConfigurations */

		  USB_CONFIGUARTION_DESC_SIZE, /* bLength */
		  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType */
		  WBVAL( /* wTotalLength */
		  1*USB_CONFIGUARTION_DESC_SIZE + 1*USB_INTERFACE_DESC_SIZE + DFU_FUNC_DESC_SIZE),
		  		0x01, /* bNumInterfaces */
		  		0x03, /* bConfigurationValue */
		  		0x00, /* iConfiguration */
		  		USB_CONFIG_SELF_POWERED /*|*//* bmAttributes */
		  		/*USB_CONFIG_REMOTE_WAKEUP*/, USB_CONFIG_POWER_MA(100), /* bMaxPower */
		  		/* Interface 0, Alternate Setting 0, DFU Class */
		  		USB_INTERFACE_DESC_SIZE, /* bLength */
		  		USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
		  		0x00, /* bInterfaceNumber */
		  		0x00, /* bAlternateSetting */
		  		0x00, /* bNumEndpoints */
		  		USB_DEVICE_CLASS_APP, /* bInterfaceClass */
		  		USB_DFU_SUBCLASS, /* bInterfaceSubClass */
		  		0x02, /* bInterfaceProtocol */
		  		0x00, /* iInterface */
		  		/* DFU RunTime/DFU Mode Functional Descriptor */
		  		DFU_FUNC_DESC_SIZE, /* bLength */
		  		USB_DFU_DESCRIPTOR_TYPE, /* bDescriptorType */
		  		USB_DFU_CAN_DOWNLOAD | USB_DFU_CAN_UPLOAD | USB_DFU_MANIFEST_TOL, /* bmAttributes */
		  		WBVAL(0xFF00), /* wDetachTimeout */
		  		WBVAL(USB_DFU_XFER_SIZE), /* wTransferSize */
		  		WBVAL(0x100), /* bcdDFUVersion */

		    0x04,                              /* bLength */
		    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
		    WBVAL(0x0409), /* US English */    /* wLANGID */
		    /* Index 0x01: Manufacturer */
		    (18*2 + 2),                        /* bLength (13 Char + Type + lenght) */
		    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
		    'N', 0,
		    'X', 0,
		    'P', 0,
		    ' ', 0,
		    'S', 0,
		    'e', 0,
		    'm', 0,
		    'i', 0,
		    'c', 0,
		    'o', 0,
		    'n', 0,
		    'd', 0,
		    'u', 0,
		    'c', 0,
		    't', 0,
		    'o', 0,
		    'r', 0,
		    's', 0,
		    /* Index 0x02: Product */
		    (14*2 + 2),                        /* bLength (13 Char + Type + lenght) */
		    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
		    'L', 0,
		    'P', 0,
		    'C', 0,
		    '1', 0,
		    '7', 0,
		    'x', 0,
		    'x', 0,
		    ' ', 0,
		    'M', 0,
		    'e', 0,
		    'm', 0,
		    'o', 0,
		    'r', 0,
		    'y', 0,
		    /* Index 0x03: Serial Number */
		    (13*2 + 2),                        /* bLength (13 Char + Type + lenght) */
		    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
		    'A', 0,
		    'B', 0,
		    'C', 0,
		    'D', 0,
		    '1', 0,
		    '2', 0,
		    '3', 0,
		    '4', 0,
		    '5', 0,
		    '6', 0,
		    '7', 0,
		    '8', 0,
		    '9', 0,
		    /* Index 0x04: Interface 0, Alternate Setting 0 */
		    (3*2 + 2),                        /* bLength (3 Char + Type + lenght) */
		    USB_STRING_DESCRIPTOR_TYPE,       /* bDescriptorType */
		    'D', 0,
		    'F', 0,
		    'U', 0,
		    /* Index 0x05: Interface 1, Alternate Setting 0 */
		    (6*2 + 2),                        /* bLength (13 Char + Type + lenght) */
		    USB_STRING_DESCRIPTOR_TYPE,       /* bDescriptorType */
		    'M', 0,
		    'e', 0,
		    'm', 0,
		    'o', 0,
		    'r', 0,
		    'y', 0,



// terminating zero
	0
};
//static const U8 abDescriptors[] = {
//
//// device descriptor
//	0x12,
//	DESC_DEVICE,
//	LE_WORD(0x0200),			// bcdUSB
//	0x00,						// bDeviceClass
//	0x00,						// bDeviceSubClass
//	0x00,						// bDeviceProtocol
//	MAX_PACKET_SIZE0,			// bMaxPacketSize
//	LE_WORD(0xFFFF),			// idVendor
//	LE_WORD(0x0005),			// idProduct
//	LE_WORD(0x0100),			// bcdDevice
//	0x01,						// iManufacturer
//	0x02,						// iProduct
//	0x03,						// iSerialNumber
//	0x01,						// bNumConfigurations
//
//		// configuration descriptor
//			0x09,
//			DESC_CONFIGURATION,
//			LE_WORD(32),				// wTotalLength
//			0x01,						// bNumInterfaces
//			0x01,						// bConfigurationValue
//			0x00,						// iConfiguration
//			0xC0,						// bmAttributes
//			0x32,						// bMaxPower
//
//
//			// data class interface descriptor
//					0x09,
//					DESC_INTERFACE,
//					0x00,						// bInterfaceNumber
//					0x00,						// bAlternateSetting
//					0x02,						// bNumEndPoints
//					0x0A,						// bInterfaceClass = data
//					0x00,						// bInterfaceSubClass
//					0x00,						// bInterfaceProtocol
//					0x00,						// iInterface
//						// data EP OUT
//							0x07,
//							DESC_ENDPOINT,
//							BULK_OUT_EP,				// bEndpointAddress
//							0x02,						// bmAttributes = bulk
//							LE_WORD(MAX_PACKET_SIZE),	// wMaxPacketSize
//							0x00,						// bInterval
//
//							// data EP in
//							0x07,
//							DESC_ENDPOINT,
//							BULK_IN_EP,					// bEndpointAddress
//							0x02,						// bmAttributes = bulk
//							LE_WORD(MAX_PACKET_SIZE),	// wMaxPacketSize
//							0x00,						// bInterval
//
//	// string descriptors
//	0x04,
//	DESC_STRING,
//	LE_WORD(0x0409),
//
//	0x0E,
//	DESC_STRING,
//	'L', 0, 'P', 0, 'C', 0, 'U', 0, 'S', 0, 'B', 0,
//
//	25,
//	DESC_STRING,
//	'D',0,'M',0,'X',0, ' ',0, 'I',0,'n',0,'t',0,'e',0,'r',0,'f',0,'a',0,'c',0,'e',
//
//	0x12,
//	DESC_STRING,
//	'D', 0, 'E', 0, 'A', 0, 'D', 0, 'C', 0, '0', 0, 'D', 0, 'E', 0,
//
//// terminating zero
//	0
//};
