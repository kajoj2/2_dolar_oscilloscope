#include "stm32f10x.h"

#include "usb_lib.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "command.h"
#include "buzzer.h"
#include "armscope_settings.h"
#include <string.h>

int buzz = 0;
static const char szId[] = "miniscope_v2c 20120414";
/** This is data added as header to each frame that isn't actual data but that is reply
	to some host request.
 */
static const unsigned char replyHeader[] =
{
		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF
};

struct S_ARMSCOPE_SETTINGS settings;

enum E_ACTION_REQUEST action = REQUEST_NONE;

struct Data {
	uint16_t *data;
	int len;
} new_data;

extern uint8_t USART_Rx_Buffer[USART_RX_DATA_SIZE];
extern uint32_t USART_Rx_ptr_in;
extern uint32_t USART_Rx_ptr_out;

int main(void)
{
	Set_System();
#if 1
	// give audible signal
	for (int i=0; i<2; i++)
	{
		beep();
	}
#endif
	Set_USBClock();
	USB_Interrupts_Config();
	USB_Init();

	new_data.data = NULL;
	new_data.len = 0;

	uint16_t *data = NULL;

	while (1)
	{
		if (buzz > 0)
		{
			buzz--;
			Buzzer_On();
		}
		else
		{
			Buzzer_Off();
		}

		if (action == REQUEST_IDENTIFY)
		{
			action = REQUEST_NONE;

			Buzzer_Off();
			// USART_Rx_ptr_in = USART_Rx_ptr_out = 0;

			buzz += 30000;
			// insert reply header
			UsbSendData(replyHeader, sizeof(replyHeader));
			int len = sizeof(replyHeader);
			while (len + strlen(szId) < 4096)
			{
				UsbSendData((const unsigned char*)szId, strlen(szId));
				len += strlen(szId);
			}
			// pad reply to same size as regular data packet
			for (int i = 0; i < 4096 - len; i++)
			{
				USART_Rx_Buffer[USART_Rx_ptr_in] = 0;
				USART_Rx_ptr_in++;

				/* To avoid buffer overflow */
				if (USART_Rx_ptr_in == USART_RX_DATA_SIZE)
				{
					USART_Rx_ptr_in = 0;
				}
			}
			//// delay - make sure that reply would not be overwritten by new samples
			//for(volatile int i=0;i<0x80000;i++);
		}

		if (new_data.data != data)
		{
			data = new_data.data;
			int len = new_data.len;

			unsigned char *ptr = (unsigned char*)data + 1;
			for (int i = 0; i < len; i++)
			{
				USART_Rx_Buffer[USART_Rx_ptr_in++] = *ptr; //*(((unsigned char*)&data[i]) + 1);
				ptr += 2;

				/* To avoid buffer overflow */
				if (USART_Rx_ptr_in == USART_RX_DATA_SIZE)
				{
					USART_Rx_ptr_in = 0;
				}
#if 0
				if (USART_Rx_ptr_out == USART_Rx_ptr_in)
				{
					// overflow debug
					buzz += 1024;
				}
#endif
			}
		}
	}
}

void OnUsbDataRx(uint8_t* dataIn, uint8_t length)
{
	if (length>=3)
	{
		if (dataIn[2]==0x00)	//end of message detected - correct frame
		{
			enum E_ACTION_REQUEST tmp = ExecuteCmd(dataIn, length, &settings);
			if (tmp != REQUEST_NONE)
			{
				action = tmp;
			}
		}
		length=0;
	}
}


void OnAdcData(uint16_t *data, int len)
{
	new_data.data = data;
	new_data.len = len;
}

void assert_failed(const char* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	Buzzer_On();
	/* Infinite loop */
	while (1)
	{}
}

