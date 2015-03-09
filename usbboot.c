#include <lpc17xx.h>
#include <lpc17xx_systick.h>

#include <usbapi.h>
#include "usb_desc.h"
#include "usbboot.h"

#include <printf.h>
#include "sbl_iap.h"
#include "sbl_config.h"

uint8_t flashing;
uint32_t size = 0;

static const unsigned key = 0x2eb5040b;

static uint32_t lfsr = 1;

static void srand(uint32_t seed) {
	if(!seed) seed = 1;
	lfsr = seed;
}

static uint32_t rand() {
	lfsr = (lfsr >> 1) ^ (-(lfsr & 1u) & 0xD0000001u);
	return lfsr;
}

static uint8_t equals(char* a, char* b, int len) {
	int i;
	for(i = 0; i < len; i++) {
		if(a[i] != b[i]) return 0;
	}
	return 1;
}

uint16_t crc16_update(uint16_t crc, uint8_t a) {
	int i;
	crc ^= a;
	for (i = 0; i < 8; ++i) {
		if (crc & 1)
			crc = (crc >> 1) ^ 0xA001;
		else
			crc = (crc >> 1);
	}
	return crc;
}

uint32_t offset = 0;
//uint32_t

extern volatile uint8_t boot_flag;


U8 dfu_data[USB_DFU_XFER_SIZE];
U8 dfu_state = DFU_STATE_dfuIDLE;
uint8_t complete;

BOOL dfu_req_handler(TSetupPacket *pSetup, int *piLen, U8 **ppbData) {
	static uint32_t block_num = 0;
	static uint32_t packet_len = 0;
	static uint8_t buf[USB_DFU_XFER_SIZE];
	static const uint8_t* pbuf = buf;

	static uint8_t dfu_status;
	static DFU_STATUS_T dfu_req_get_status;

//	printf("ev %d bmRequestType:%x bRequest:%x\n", event, pCtrl->SetupPacket.bmRequestType.B,
//			pCtrl->SetupPacket.bRequest);

//	if(event == USB_EVT_RESET && complete) {
//		usb_connect(0);
//		//app_exec(0x1000);
//
//		NVIC_SystemReset();
//	}

//	if(event == USB_EVT_OUT) {
//		if(pCtrl->SetupPacket.bRequest == USB_REQ_DFU_DNLOAD) {
//			pCtrl->EP0Data.Count = 0;
//	        pUsbApi->core->StatusInStage(hUsb);
//			return LPC_OK;
//		}
//	}
	//printf("ev%d\n", pSetup->bRequest);
	if((pSetup->bmRequestType & 0x21)) {
		//if(p)
		//printf("%d\n", p->dfu_state);
		switch(pSetup->bRequest) {
		case USB_REQ_DFU_DETACH:

			//pUsbApi->core->DataInStage(hUsb);
			return 1;
		case USB_REQ_DFU_GETSTATUS:
			//printf("getstatus %d\n", p->dfu_state);
			if(dfu_state == DFU_STATE_dfuDNLOAD_SYNC) {

				//bwPollTimeout[0] = 255;
				//bwPollTimeout[1] = 255;
				//  bwPollTimeout[2] = 255;
				if (packet_len != 0) {
					uint32_t dest_addr = DFU_DEST_BASE;

					if (block_num >= DFU_MAX_BLOCKS) {
						dfu_state = DFU_STATUS_errADDRESS;
					} else {

						dest_addr += (block_num * USB_DFU_XFER_SIZE);

						write_flash((unsigned*) dest_addr, buf,
								packet_len, (packet_len < USB_DFU_XFER_SIZE) ? 1 : 0);
					}
				}

				if(packet_len == 0) {
					dfu_state = DFU_STATE_dfuIDLE;
					complete = 1;
				} else {
					dfu_state = DFU_STATE_dfuDNLOAD_IDLE;
				}
			}
			dfu_req_get_status.bState = dfu_state;
			dfu_req_get_status.bStatus = dfu_status;
			dfu_req_get_status.iString = 0;
			dfu_req_get_status.bwPollTimeout[0] = 255;

			*ppbData = &dfu_req_get_status;
			*piLen = sizeof(dfu_req_get_status);
	        return 1;

		break;
		case USB_REQ_DFU_GETSTATE:
			*ppbData[0] = dfu_state;
			*piLen = 1;
	        return 1;

		case USB_REQ_DFU_UPLOAD:
		{
			block_num = pSetup->wValue;
			dfu_state = DFU_STATE_dfuUPLOAD_IDLE;
			uint8_t* src_addr = DFU_DEST_BASE + (block_num * USB_DFU_XFER_SIZE);
		    //*pBuff = (uint8_t*)src_addr;
			//pSetup->EP0Data.pData = pCtrl->EP0Buf;
			//printf("upl %d %d\n", block_num, pSetup->wLength);
		    if (block_num == DFU_MAX_BLOCKS) {
		    	*piLen = 0;
		    	dfu_state = DFU_STATE_dfuIDLE;
		    } else {
			    if (block_num > DFU_MAX_BLOCKS) {
			    	*piLen = 0;
			        dfu_state = DFU_STATUS_errADDRESS;
			    } else {
			    	memcpy(*ppbData, (void*)src_addr, pSetup->wLength);
			    	*piLen = pSetup->wLength;
			    }
		    }

			return 1;
		}
		case USB_REQ_DFU_DNLOAD:
			dfu_state = DFU_STATE_dfuDNLOAD_SYNC;
			block_num = pSetup->wValue;
			packet_len = pSetup->wLength;

			memcpy(buf, *ppbData, packet_len);
			return 1;
		break;
		case USB_REQ_DFU_ABORT:
			dfu_state = DFU_STATE_dfuIDLE;
			//pCtrl->EP0Data.Count = 0;
	        //pUsbApi->core->DataInStage(hUsb);
	        return 1;
		//default:
			//printf("unk request %d %d\n", pSetup->bRequest);

		}
	}
	return 1;
}


static void HandleUsbReset(U8 bDevStatus)
{
	if (bDevStatus & DEV_STATUS_RESET) {
		if(complete) {
			usbConnect(FALSE);
			NVIC_SystemReset();
		}
	}
}

void usb_boot_init() {
	USBHwRegisterDevIntHandler(HandleUsbReset);
	USBRegisterRequestHandler(REQUEST_CLASS, dfu_req_handler, dfu_data);

}
/*
 * usbboot.c
 *
 *  Created on: Jan 14, 2013
 *      Author: walmis
 */


