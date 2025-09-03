/*
 * Bluetooth_Driver.h
 *
 *  Created on: 2 sep. 2025
 *      Author: Usuario
 */

#ifndef BLUETOOTH_DRIVER_H_
#define BLUETOOTH_DRIVER_H_

/** @defgroup Devices_Library
 *  @ingroup "Biopoteciales-LIRINS"
 *  Driver for controlling the HC05 bluetooth module.
 *  @{
 */

// INCLUDES IMPORTANTES
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_usart.h"
#include "fsl_gpio.h"
#include "fsl_iocon.h"
#include "clock_config.h"
#include "board.h"
#include <string.h>
#include <stdio.h>

/**
 * Definitions and Declarations
 */

// DEFINES IMPORTANTES
#define USART_CLK		kCLOCK_MainClk
#define USART_CLK_FREQ  CLOCK_GetFreq(USART_CLK)


// DEFINICÓN DE LAS FUNCIONES

/**
 * @brief Function to initialize the USART port and Rx interrupts
 * @param baudrate Baudrate number for communication with other devices
 */
void HC05_Init(uint32_t baudrate);

/**
 * @brief Function to send data trough the bluetooth module
 * @param data Data to send
 */
void HC05_SendString(const uint8_t *data);
void HC05_SendBytes(const uint8_t *ptr_packet, uint8_t dataLength);

/**
 * @brief Function to enable the USART interrupt linked to kUSART_RxReadyInterruptEnable. The interrupt will trigger when USART had a
 * not read byte in Rx port
 */
void USART_SetupInterrupts();

/**
* @}
*/


#endif /* BLUETOOTH_DRIVER_H_ */
