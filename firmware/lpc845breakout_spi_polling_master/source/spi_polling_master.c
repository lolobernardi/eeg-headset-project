/*
 * Copyright  2017 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_spi.h"
#include "board.h"
#include "app.h"
#include "ADS1299_Driver.h"
#include "fsl_debug_console.h"
#include "ads1299_test.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

int main(void)
{
    /* Initizlize the hardware(clock/pins configuration/debug console). */
    BOARD_InitHardware();

    PRINTF("This is SPI polling transfer master example.\n\r");
    PRINTF("\n\rMaster start to send data to slave, please make sure the slave has been started!\n\r");

    ADS1299_Init();

    //prueba_ADS_SPI();
    ADS1299_RunRegisterTest();

    while (1)
    {
    }
}


