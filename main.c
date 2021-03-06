//#include <lpcxx.h>

//#define NEW_HW


#include <lpc17xx.h>
#include <lpc17xx_systick.h>
#include <lpc17xx_clkpwr.h>
#include <lpc17xx_uart.h>
#include <lpc17xx_pinsel.h>
#include <lpc17xx_clkpwr.h>
#include <core_cm3.h>

#include <alloca.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sys/printf.h"

#include <usbapi.h>


#include "sbl_iap.h"
#include "sbl_config.h"

#include "usb_desc.h"
#include "usbboot.h"

#include "config.h"


#define SET_LED(led) (LED_PORT->FIOSET |= (1<<led))
#define CLR_LED(led) (LED_PORT->FIOCLR |= (1<<led))
#define TOGGLE_LED(led) (LED_PORT->FIOPIN ^= (1<<led))

#define DBG

#define CRP1  0x12345678
#define CRP2  0x87654321
#define CRP3  0x43218765
#define NOCRP 0x11223344

const uint32_t crp __attribute__((section(".crp"))) = NOCRP;

volatile uint32_t time_ms;

void SysTick_Handler(void) {
	SYSTICK_ClearCounterFlag();

	LPC_WDT->WDFEED = 0xAA;
	LPC_WDT->WDFEED = 0x55;

	time_ms++;
}

void delay_busy(uint32_t time) {
	uint32_t new_time = time_ms + time;

	while(time_ms < new_time);
}

//usb interrupt handler
void USB_IRQHandler() {
	USBHwISR();
}



void uart_init() {

	UART_CFG_Type cfg;
	UART_FIFO_CFG_Type fifo_cfg;

	UART_ConfigStructInit(&cfg);
	cfg.Baud_rate = 57600;

	PINSEL_CFG_Type PinCfg;

	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 2;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 3;
	PINSEL_ConfigPin(&PinCfg);

	UART_Init(LPC_UART0, &cfg);

	UART_FIFOConfigStructInit(&fifo_cfg);

	UART_FIFOConfig(LPC_UART0, &fifo_cfg);

	UART_TxCmd(LPC_UART0, ENABLE);

}

volatile uint8_t boot_flag = 0;

void usbConnect(uint8_t con) {
	USBHwConnect(con);

#ifdef USB_CONNECT_PORT
#ifndef USB_CONNECT_INVERT
	if(con) {
		USB_CONNECT_PORT->FIOSET |= 1 << USB_CONNECT_PIN;
	} else {
		USB_CONNECT_PORT->FIOCLR |= 1 << USB_CONNECT_PIN;
	}
#else
	if(con) {
		USB_CONNECT_PORT->FIOCLR |= 1 << USB_CONNECT_PIN;
	} else {
		USB_CONNECT_PORT->FIOSET |= 1 << USB_CONNECT_PIN;
	}
#endif

#endif


}

void vcom_putc(void* a, char c) {
	while (!(LPC_UART0->LSR & UART_LSR_THRE));
	LPC_UART0->THR = c;
}

extern U8 dfu_state;

int main() {
	BOOT_INIT;

	#ifdef LED_PORT
		LED_PORT->FIODIR |= 1<<LED_PIN;
	#endif
		//init_printf(NULL, vcom_putc);
		//printf("a\n");
//	#ifdef LED_PORT
//		LED_PORT->FIOCLR |= 1<<LED_PIN;
//	#endif

	int rstsrc = LPC_SC->RSID & 0xF;
	uint8_t wdreset = (rstsrc & (1<<2)) /*|| rstsrc == 0*/;

	LPC_SC->RSID = 0xF;

#ifdef P2_10_RESET
	if(!(LPC_GPIO2->FIOPIN & 1<<10)) {
		LPC_SC->RSID = 0xF;
		NVIC_SystemReset();
	}
#endif

	if(!BOOT_ENTRY_CONDITION) {
		if(!wdreset && user_code_present()
#ifdef P2_10_RESET
		&& !(LPC_GPIO2->FIODIR & (1<<10))
#endif

		) {
			//delay_busy(1000);
			execute_user_code();
		}
	}

	SystemInit();
	_segs_init();

	//uart_init();
	//init_printf(NULL, vcom_putc);

	SYSTICK_InternalInit(1);
	SYSTICK_IntCmd(ENABLE);
	SYSTICK_Cmd(ENABLE);

#ifdef USB_CONNECT_PORT
	USB_CONNECT_PORT->FIODIR |= 1<<USB_CONNECT_PIN;
#ifdef USB_CONNECT_INVERT
	USB_CONNECT_PORT->FIOSET |= 1<<USB_CONNECT_PIN;
#endif
#endif

	USBInit();

	USBRegisterDescriptors(abDescriptors);
	//USBHwRegisterDevIntHandler(on_usb_device_status);

	usb_boot_init();

	usbConnect(TRUE);

	NVIC_EnableIRQ(USB_IRQn);

	//printf("labas\n");
	uint8_t timeout = 50;

	while(1) {

		//LED_PORT->FIOSET |= 1<<LED_PIN;


#ifdef LED_PORT
		delay_busy(80);
		LED_PORT->FIOSET |= 1<<LED_PIN;
		delay_busy(80);
		LED_PORT->FIOCLR |= 1<<LED_PIN;
#else
		delay_busy(160);
#endif
		timeout--;
		if(timeout == 0 && wdreset && dfu_state == DFU_STATE_dfuIDLE) {
			//LPC_SC->RSID |= 1<<2; //clear wd reset bit
			usbConnect(FALSE);
			NVIC_SystemReset();
		}

#ifdef P2_10_RESET
		// if P2.10 is low, reset the system to enter ISP
		if(!(LPC_GPIO2->FIOPIN & 1<<10)) {
			NVIC_SystemReset();
		}
#endif


	}
}
