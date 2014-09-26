#pragma once

// Config for QuadController project /////

#define BOOT_INIT

#define USB_CONNECT_PORT LPC_GPIO1
#define USB_CONNECT_PIN 30
//#define USB_CONNECT_INVERT

#define LED_PORT LPC_GPIO0
#define LED_PIN 25

#define USER_START_SECTOR 1
#define MAX_USER_SECTOR 21

#define BOOT_ENTRY_CONDITION (0)

