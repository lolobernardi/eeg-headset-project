/*
 * hc05.c
 * PINES HC05 || PINES LPC485
 *	 STATE	  || P0_31 (GPIO de entrada)
 *	  RX	  || RX de USART0 (P0_29)
 *	  TX	  || RX de USART0 (P0_30)
 *	  GND	  || GND
 *	 +5V	  || VDD
 *
 *
 *  Created on: 6 ago. 2025
 *      Author: Lorenzo Bernardi y Francisco Dodera
 */

#include "Bluetooth_Driver.h"
//#include "openbci_communication.h"

// DELAY
static inline void delay_ms(uint32_t ms)
{
    SDK_DelayAtLeastUs(ms * 1000U, SystemCoreClock);
}

// FUNCTIONS
/**
 * @brief Function to initialize the USART port and Rx interrupts
 * @param baudrate Baudrate number for communication with other devices
 */
void HC05_Init(uint32_t baudrate)
{
    usart_config_t config;
    USART_GetDefaultConfig(&config);
    config.baudRate_Bps = baudrate;
    config.enableTx = true;
    config.enableRx = true;
    USART_Init(USART0, &config, USART_CLK_FREQ);

    USART_SetupInterrupts();
}

/**
 * @brief Function to send data trough the bluetooth module
 * @param data Data to send
 */
void HC05_SendString(const uint8_t *data) {
    USART_WriteBlocking(USART0, data, strlen(data));
    //delay_ms(10);
}

void HC05_SendBytes(const uint8_t *ptr_packet, uint8_t dataLength)
{
	USART_WriteBlocking(USART0, ptr_packet, dataLength);
}

/**
 * @brief Function to enable the USART interrupt linked to kUSART_RxReadyInterruptEnable. The interrupt will trigger when USART had a
 * not read byte in Rx port
 */
void USART_SetupInterrupts(void) {
    USART_EnableInterrupts(USART0, kUSART_RxReadyInterruptEnable);
    NVIC_EnableIRQ(USART0_IRQn);
}

/**
 * @brief Function liked to the interruption. It checks if the byte has been received with the flag kUSART_RxReady and then reads that byte
 *  with USART_ReadByte, which also cleans the flag
 */
void USART0_IRQHandler(void) {
	//Check if data has been received
	if (USART_GetStatusFlags(USART0) & kUSART_RxReady)
	{
		uint8_t receivedByte = USART_ReadByte(USART0);

		//Responds to commands sent by the IDE
		EventSerial(receivedByte);
	}
}
