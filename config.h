#pragma once

// Config for QuadController project /////

#define BOOT_INIT

#define USB_CONNECT_PORT LPC_GPIO1
#define USB_CONNECT_PIN 18
//#define USB_CONNECT_INVERT

#define P2_10_RESET

#define LED_PORT LPC_GPIO1
#define LED_PIN 0

#define USER_START_SECTOR 1
#define MAX_USER_SECTOR 29

#define BOOT_ENTRY_CONDITION (0)


#define IDVENDOR 0xFFFF
#define IDPRODUCT 0xf3c4

#define DFU_DEST_BASE 0x1000
#define DFU_MAX_IMAGE_LEN     (508 * 1024)
#define DFU_MAX_BLOCKS        (DFU_MAX_IMAGE_LEN/USB_DFU_XFER_SIZE)
//#define WD_ENABLE
