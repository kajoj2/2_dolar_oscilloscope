/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
 * File Name          : hw_config.c
 * Author             : MCD Application Team
 * Version            : V3.3.0
 * Date               : 21-March-2011
 * Description        : Hardware Configuration & Setup
 ********************************************************************************
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
 * AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
 * CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
 * INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *******************************************************************************/

#include "stm32f10x_it.h"

#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "platform_config.h"
#include "usb_pwr.h"

ErrorStatus HSEStartUpStatus;
USART_InitTypeDef USART_InitStructure;

uint8_t USART_Rx_Buffer[USART_RX_DATA_SIZE];
uint32_t USART_Rx_ptr_in = 0;
uint32_t USART_Rx_ptr_out = 0;
uint32_t USART_Rx_length = 0;

static void IntToUnicode(uint32_t value, uint8_t *pbuf, uint8_t len);

#define ADC1_DR_Address    ((uint32_t)0x4001244C)
enum { ADC_HALF_BUFFER = 512 };
uint16_t ADCbuf[2*ADC_HALF_BUFFER];

// Remap the ADC DMA handler
void DMA1_Channel1_IRQHandler() __attribute__ ((alias("ADC_DMA_Handler")));

/*******************************************************************************
 * Description    : Configures Main system clocks & power
 *******************************************************************************/
void Set_System(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	/* Enable DMA1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* Enable ADC1, ADC2 and buzzer port GPIO clock */
	RCC_APB2PeriphClockCmd(
			RCC_APB2Periph_ADC1 |
			RCC_APB2Periph_ADC2 |
			RCC_APB2Periph_GPIO_BUZZER,
			ENABLE);

	GPIO_InitStructure.GPIO_Pin = BUZZER_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(BUZZER_PORT, &GPIO_InitStructure);

	/* Configure PA0 and PA1 as analog input -------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* DMA1 channel1 configuration ----------------------------------------------*/
	DMA_InitTypeDef DMA_InitStructure;
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) & ADCbuf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = sizeof(ADCbuf) / sizeof(ADCbuf[0]) / 2;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	/* Enable DMA1 channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
	/* Trigger interrupt when for half conversions too to indicate double buffer */
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
	DMA_ITConfig(DMA1_Channel1, DMA_IT_HT, ENABLE);

	ADC_InitTypeDef ADC_InitStructure;
	/* ADC1 and ADC2 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Left;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_Init(ADC2, &ADC_InitStructure);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_13Cycles5);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_1, 1, ADC_SampleTime_13Cycles5);
	/* Enable ADC2 external trigger conversion (to be controlled by ADC1)*/
	ADC_ExternalTrigConvCmd(ADC2, ENABLE);
	ADC_DMACmd(ADC1, ENABLE); /* Enable ADC1 DMA */
	ADC_Cmd(ADC1, ENABLE); /* Enable ADC1 */
	ADC_Cmd(ADC2, ENABLE); /* Enable ADC2 */

	ADC_ResetCalibration(ADC1); /* Enable ADC1 reset calibration register */
	/* Check the end of ADC1 reset calibration register */
	while (ADC_GetResetCalibrationStatus(ADC1)) {};
	/* Start ADC1 calibration */
	ADC_StartCalibration(ADC1);
	/* Check the end of ADC1 calibration */
	while (ADC_GetCalibrationStatus(ADC1)) {};

	ADC_ResetCalibration(ADC2); /* Enable ADC2 reset calibration register */
	/* Check the end of ADC2 reset calibration register */
	while (ADC_GetResetCalibrationStatus(ADC2)) {};
	/* Start ADC2 calibration */
	ADC_StartCalibration(ADC2);
	/* Check the end of ADC2 calibration */
	while (ADC_GetCalibrationStatus(ADC2)) {};

	/* Configure and enable DMA interrupt */
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Start ADC1 Software Conversion */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

extern void assert_failed(const char* file, uint32_t line);
extern void OnAdcData(uint16_t *data, int len);

void ADC_DMA_Handler(void)
{
	if (DMA_GetFlagStatus(DMA1_IT_TC1))
	{
		// whole double buffer filled
		OnAdcData(ADCbuf + ADC_HALF_BUFFER, ADC_HALF_BUFFER);
	}
	else if (DMA_GetFlagStatus(DMA1_IT_HT1))
	{
		OnAdcData(ADCbuf, ADC_HALF_BUFFER);
	}
	else
	{
		assert_failed(__FILE__, __LINE__);
	}

	// Clear all interrupt from DMA 1 - regardless if buffer swapped
	DMA_ClearFlag(DMA1_IT_GL1);
}

void Buzzer_On(void)
{
	BUZZER_PORT->ODR |= BUZZER_PIN;
}

void Buzzer_Off(void)
{
	BUZZER_PORT->ODR &= ~BUZZER_PIN;
}

/*******************************************************************************
 * Description    : Configures USB Clock input (48MHz)
 *******************************************************************************/
void Set_USBClock(void)
{
	/* Select USBCLK source */
	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);

	/* Enable the USB clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
}

/*******************************************************************************
 * Description    : Power-off system clocks and power while entering suspend mode
 *******************************************************************************/
void Enter_LowPowerMode(void)
{
	/* Set the device state to suspend */
	bDeviceState = SUSPENDED;
}

/*******************************************************************************
 * Description    : Restores system clocks and power while exiting suspend mode
 *******************************************************************************/
void Leave_LowPowerMode(void)
{
	DEVICE_INFO *pInfo = &Device_Info;

	/* Set the device state to the correct state */
	if (pInfo->Current_Configuration != 0)
	{
		/* Device configured */
		bDeviceState = CONFIGURED;
	}
	else
	{
		bDeviceState = ATTACHED;
	}
}

/*******************************************************************************
 * Description    : Configures the USB interrupts
 *******************************************************************************/
void USB_Interrupts_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
 * Description    : Software Connection/Disconnection of USB Cable
 *******************************************************************************/
void USB_Cable_Config(FunctionalState NewState)
{
}

void UsbSendData(const unsigned char* data, int len)
{
	for (int i = 0; i < len; i++)
	{
		USART_Rx_Buffer[USART_Rx_ptr_in] = data[i];
		USART_Rx_ptr_in++;

		/* To avoid buffer overflow */
		if (USART_Rx_ptr_in == USART_RX_DATA_SIZE)
		{
			USART_Rx_ptr_in = 0;
		}
	}
}


/*******************************************************************************
 * Description    : Create the serial number string descriptor.
 *******************************************************************************/
void Get_SerialNum(void)
{
	uint32_t Device_Serial0, Device_Serial1, Device_Serial2;

	Device_Serial0 = *(__IO uint32_t*) (0x1FFFF7E8);
	Device_Serial1 = *(__IO uint32_t*) (0x1FFFF7EC);
	Device_Serial2 = *(__IO uint32_t*) (0x1FFFF7F0);

	Device_Serial0 += Device_Serial2;

	if (Device_Serial0 != 0)
	{
		IntToUnicode(Device_Serial0, &Virtual_Com_Port_StringSerial[2], 8);
		IntToUnicode(Device_Serial1, &Virtual_Com_Port_StringSerial[18], 4);
	}
}

/*******************************************************************************
 * Description    : Convert Hex 32Bits value into char.
 *******************************************************************************/
static void IntToUnicode(uint32_t value, uint8_t *pbuf, uint8_t len)
{
	uint8_t idx = 0;

	for (idx = 0; idx < len; idx++)
	{
		if (((value >> 28)) < 0xA)
		{
			pbuf[2 * idx] = (value >> 28) + '0';
		}
		else
		{
			pbuf[2 * idx] = (value >> 28) + 'A' - 10;
		}

		value = value << 4;

		pbuf[2 * idx + 1] = 0;
	}
}
