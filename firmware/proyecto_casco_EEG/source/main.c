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
#include "openbci_communication.h"
#include "Bluetooth_Driver.h"
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
    /* Initialize the hardware(clock/pins configuration/debug console). */
    BOARD_InitHardware();
    ADS1299_Init();
    HC05_Init(115200);
    //init_millis();

    while (1)
    {
    	SendChannelData();
    }
}


