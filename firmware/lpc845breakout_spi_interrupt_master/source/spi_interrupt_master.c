/*
 * Copyright  2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_spi.h"
#include "board.h"
#include "app.h"
#include "fsl_debug_console.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void EXAMPLE_SPIMasterInit(void);
static void EXAMPLE_MasterStartTransfer(void);
static void EXAMPLE_TransferDataCheck(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
#define BUFFER_SIZE (64)
static uint8_t txBuffer[BUFFER_SIZE];
static uint8_t rxBuffer[BUFFER_SIZE];
static uint32_t txIndex            = BUFFER_SIZE;
static uint32_t rxIndex            = BUFFER_SIZE;
static volatile bool slaveFinished = false;

/*******************************************************************************
 * Code
 ******************************************************************************/

void SPI_MASTER_IRQHandler(void)
{
    /* Check if data is ready in RX register. */
    if ((SPI_GetStatusFlags(EXAMPLE_SPI_MASTER) & kSPI_RxReadyFlag))
    {
        rxBuffer[BUFFER_SIZE - rxIndex] = SPI_ReadData(EXAMPLE_SPI_MASTER);
        rxIndex--;
    }
    /* Write data to TX regsiter if TX register is ready. */
    if ((SPI_GetStatusFlags(EXAMPLE_SPI_MASTER) & kSPI_TxReadyFlag) && (txIndex != 0U))
    {
        /* If this is the last byte to send. */
        if (1U == txIndex)
        {
            /* Add end of transfer configuration. */
            SPI_WriteConfigFlags(EXAMPLE_SPI_MASTER, kSPI_EndOfTransfer);
            SPI_WriteData(EXAMPLE_SPI_MASTER, txBuffer[BUFFER_SIZE - txIndex]);
        }
        else
        {
            SPI_WriteData(EXAMPLE_SPI_MASTER, (uint16_t)(txBuffer[BUFFER_SIZE - txIndex]));
        }
        txIndex--;
    }
    /* If no data to be transferred, disable the interrupt. */
    if ((txIndex == 0U) && (rxIndex == 0U))
    {
        slaveFinished = true;
        SPI_DisableInterrupts(EXAMPLE_SPI_MASTER, kSPI_RxReadyInterruptEnable);
    }
    SDK_ISR_EXIT_BARRIER;
}

int main(void)
{
    /* Initialize the boards */
    BOARD_InitHardware();

    PRINTF("This is SPI interrupt functional master example.\n\r");
    PRINTF("\n\rMaster start to send data to slave, please make sure the slave has been started!\n\r");

    /* Initialize the SPI master with configuration. */
    EXAMPLE_SPIMasterInit();

    /* Start transfer with slave board. */
    EXAMPLE_MasterStartTransfer();

    /* Check the received data. */
    EXAMPLE_TransferDataCheck();

    /* De-initialize the SPI master. */
    SPI_Deinit(EXAMPLE_SPI_MASTER);

    while (1)
    {
    }
}

static void EXAMPLE_SPIMasterInit(void)
{
    spi_master_config_t masterConfig = {0};
    uint32_t srcFreq                 = 0U;

    /* configuration from using SPI_MasterGetDefaultConfig():
     * userConfig->enableLoopback = false;
     * userConfig->enableMaster = true;
     * userConfig->polarity = kSPI_ClockPolarityActiveHigh;
     * userConfig->phase = kSPI_ClockPhaseFirstEdge;
     * userConfig->direction = kSPI_MsbFirst;
     * userConfig->baudRate_Bps = 500000U;
     * userConfig->dataWidth = kSPI_Data8Bits;
     * userConfig->sselNum = kSPI_Ssel0Assert;
     * userConfig->sselPol = kSPI_SpolActiveAllLow;
     */
    SPI_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate_Bps = EXAMPLE_SPI_MASTER_BAUDRATE;
    masterConfig.sselNumber   = EXAMPLE_SPI_MASTER_SSEL;
    srcFreq                   = EXAMPLE_SPI_MASTER_CLK_FREQ;
    SPI_MasterInit(EXAMPLE_SPI_MASTER, &masterConfig, srcFreq);
}

static void EXAMPLE_MasterStartTransfer(void)
{
    uint32_t i = 0U;

    /* Init source buffer */
    for (i = 0U; i < BUFFER_SIZE; i++)
    {
        txBuffer[i] = i;
        rxBuffer[i] = 0U;
    }

    /* Write data to TXDAT register to trigger receive interrupt. */
    SPI_WriteData(EXAMPLE_SPI_MASTER, txBuffer[BUFFER_SIZE - (txIndex--)]);
    /* Enable SPI receive ready interrupt. */
    EnableIRQ(EXAMPLE_SPI_MASTER_IRQ);
    SPI_EnableInterrupts(EXAMPLE_SPI_MASTER, kSPI_RxReadyInterruptEnable);
}

static void EXAMPLE_TransferDataCheck(void)
{
    uint32_t i = 0U, err = 0U;
    /* Waiting for the transmission complete. */
    while (slaveFinished != true)
    {
    }

    PRINTF("\n\rThe received data are:");
    /*Check if the data is right*/
    for (i = 0; i < BUFFER_SIZE; i++)
    {
        /* Print 16 numbers in a line */
        if ((i & 0x0FU) == 0U)
        {
            PRINTF("\n\r");
        }
        PRINTF("  0x%02X", rxBuffer[i]);
        /* Check if data matched. */
        if (txBuffer[i] != rxBuffer[i])
        {
            err++;
        }
    }

    if (err == 0)
    {
        PRINTF("\n\rMaster transfer succeed!\n\r");
    }
    else
    {
        PRINTF("\n\rMaster transfer faild!\n\r");
    }
}
