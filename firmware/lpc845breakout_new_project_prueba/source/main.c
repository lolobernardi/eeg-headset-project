
/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "peripherals.h"

#include "board.h"
#include "app.h"
#include "ADS1299_Driver.h"
#include "ads1299_test.h"
#include "fsl_usart.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_USART USART0
#define EXAMPLE_USART_CLK_SRC  kCLOCK_MainClk
#define EXAMPLE_USART_CLK_FREQ CLOCK_GetFreq(EXAMPLE_USART_CLK_SRC)


uint8_t txbuff[] =
    "Usart polling example.\r\nBoard will send back received characters.\r\nNow, please input any character:\r\n";

uint8_t txbuff2[] =
    "Anda el SPI\n";

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */

int main(void)
{

    /* Init board hardware. */
    //BOARD_InitHardware();
    //ADS1299_Init();

    uint8_t ch;
    usart_config_t config;

    BOARD_InitHardware();


    ADS1299_Init();


    USART_GetDefaultConfig(&config);
    config.enableRx     = true;
    config.enableTx     = true;
    config.baudRate_Bps = BOARD_DEBUG_USART_BAUDRATE;

    // Initialize the USART with configuration.
    USART_Init(EXAMPLE_USART, &config, EXAMPLE_USART_CLK_FREQ);

    // Send data in polling way.
    USART_WriteBlocking(EXAMPLE_USART, txbuff, sizeof(txbuff) - 1);

    //ADS1299_RunRegisterTest();

    //if(prueba_ADS_SPI())
    //	USART_WriteBlocking(EXAMPLE_USART, txbuff2, sizeof(txbuff2) - 1);



    //while (1)
    //{
    //}
}
