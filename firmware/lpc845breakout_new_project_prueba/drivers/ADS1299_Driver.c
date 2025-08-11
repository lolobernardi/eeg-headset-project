/*
 * ADS1299_Driver.c
 *
 *  Created on: 7 ago. 2025
 *      Author: Usuario
 */

#include "ADS1299_Driver.h"
#include "fsl_gpio.h"
#include "fsl_iocon.h"
#include "board.h"



static inline void delay_ms(uint32_t ms)
{
    SDK_DelayAtLeastUs(ms * 1000U, SystemCoreClock);
}

static inline void delay_us(uint32_t us)
{
    SDK_DelayAtLeastUs(us, SystemCoreClock);
}

void Drv_ADS1299_initialize(void)
{
    ADS1299_InitPins();
    ADS1299_initialize_ads();
}

void ADS1299_InitPins(void)
{
    // Variables para definir cada pin y puerto
    uint32_t ads_drdy_port = ADS_DRDY_PORT;
    uint32_t ads_drdy_pin = ADS_DRDY_PIN;

    uint32_t ads_rst_port = ADS_RST_PORT;
    uint32_t ads_rst_pin = ADS_RST_PIN;

    uint32_t ads_start_port = ADS_START_PORT;
    uint32_t ads_start_pin = ADS_START_PIN;

    uint32_t board_ads_port = BOARD_ADS_PORT;
    uint32_t board_ads_pin = BOARD_ADS_PIN;

    // uint32_t slave_ads_port = SLAVE_ADS_PORT;
    // uint32_t slave_ads_pin = SLAVE_ADS_PIN;

    gpio_pin_config_t input_config = {
        .pinDirection = kGPIO_DigitalInput,
        .outputLogic = 0U
    };

    gpio_pin_config_t output_config_high = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 1U
    };

    gpio_pin_config_t output_config_low = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };

    // Inicializar puertos GPIO necesarios
    GPIO_PortInit(GPIO, ads_drdy_port);
    GPIO_PortInit(GPIO, ads_rst_port);
    GPIO_PortInit(GPIO, ads_start_port);
    GPIO_PortInit(GPIO, board_ads_port);

    // --- BOARD ADS CHIP SELECT ---
    GPIO_PinInit(GPIO, board_ads_port, board_ads_pin, &output_config_high);

    /* // --- SLAVE ADS CHIP SELECT ---
    GPIO_PinInit(GPIO, slave_ads_port, slave_ads_pin, &output_config_high);
    */

    // --- ADS RESET ---
    GPIO_PinInit(GPIO, ads_rst_port, ads_rst_pin, &output_config_high);

    // --- ADS START ---
    GPIO_PinInit(GPIO, ads_start_port, ads_start_pin, &output_config_low);

    // --- ADS DRDY (input) ---
    GPIO_PinInit(GPIO, ads_drdy_port, ads_drdy_pin, &input_config);
}

void ADS1299_initialize_ads(void)
{
    // Delay inicial
    delay_ms(50);

    // Reset sequence: LOW -> HIGH
    GPIO_PinWrite(GPIO, ADS_RST_PORT, ADS_RST_PIN, 0); // set LOW: reset pin
    delay_us(4); // toggle reset pin
    GPIO_PinWrite(GPIO, ADS_RST_PORT, ADS_RST_PIN, 1); // set HIGH
    delay_us(20); // wait 18 Tclk before using device (aprox. 8uS)

    // Reset del ADS único
    ADS1299_resetADS(); // reset the ADS registers and stop DataContinousMode
    delay_ms(20);

    // Configurar el ADS único - sin salida de clock ya que no hay slave
    ADS1299_WREG(CONFIG1, 0x90 | SAMPLE_RATE_2kHZ); // Turn off clk output (no slave present)
    ADS1299_WREG(LOFF, 0x02);

    // Solo un ADS = 8 canales
    numChannels = 8;

    // DEFAULT CHANNEL SETTINGS FOR ADS
    defaultChannelSettings[POWER_DOWN] = NO;
    defaultChannelSettings[GAIN_SET] = ADS_GAIN24;
    defaultChannelSettings[INPUT_TYPE_SET] = ADSINPUT_NORMAL;
    defaultChannelSettings[BIAS_SET] = YES;
    defaultChannelSettings[SRB2_SET] = YES;
    defaultChannelSettings[SRB1_SET] = NO;

    uint8_t i, j = 0;
    for(i = 0; i < numChannels; i++)
    {
        for(j = 0; j < 6; j++)
        {
            channelSettings[i][j] = defaultChannelSettings[j];
        }
        useInBias[i] = TRUE;
        useSRB2[i] = TRUE;
    }
}


// Funciones para control de Chip Select
static inline void ADS1299_csLow(void)
{
    GPIO_PinWrite(GPIO, BOARD_ADS_PORT, BOARD_ADS_PIN, 0);
}

static inline void ADS1299_csHigh(void)
{
    GPIO_PinWrite(GPIO, BOARD_ADS_PORT, BOARD_ADS_PIN, 1);
}

/**
 * @brief SPI communication method
 * @param _data Byte to transfer
 * @return Byte received from SPI target
 */
uint8_t ADS1299_xfer(uint8_t _data)
{
    uint8_t rxData = 0;

    // Ejemplo usando la API de SPI del LPC845
    // Ajusta según tu configuración de SPI específica
    spi_transfer_t xfer = {0};
    xfer.txData = &_data;
    xfer.rxData = &rxData;
    xfer.dataSize = 1;

    // Asumiendo que tienes SPI configurado como SPI_MASTER_HANDLE
    // SPI_MasterTransferBlocking(SPI_MASTER_BASEADDR, &xfer);

    // O si tienes una función más simple como en tu código original:
    // rxData = spi_rw(_data);

    return rxData;
}

/**
 * @brief Set all register values to default
 */
void ADS1299_RESET(void)
{
    ADS1299_csLow();
    ADS1299_xfer(_RESET);

    delay_us(10); // After serial communication, wait 4 or more tCLK cycles before taking CS high
    ADS1299_csHigh();
    delay_us(12); // Must wait 18 tCLK cycles to execute this command (see Datasheet, pag 35)
}

/**
 * @brief Reset all the ADS1299 settings. Stops all data acquisition
 */
void ADS1299_resetADS(void)
{
    // Send RESET command to default all registers
    ADS1299_RESET();

    // Exit Read Data Continuous mode to communicate with ADS
    ADS1299_SDATAC();
    delay_ms(100);

    // Turn off all channels (solo un ADS = canales 1-8)
    for(uint8_t chan = 1; chan <= 8; chan++)
    {
        ADS1299_deactivateChannel(chan);
    }
}

/**
 * @brief Send SDATAC command (Stop Data Continuous mode)
 */
void ADS1299_SDATAC(void)
{
    ADS1299_csLow();
    ADS1299_xfer(0x11); // SDATAC command
    delay_us(10);
    ADS1299_csHigh();
    delay_us(12);
}

/**
 * @brief Deactivate a specific channel
 * @param channel Channel number (1-8)
 */
void ADS1299_deactivateChannel(uint8_t channel)
{
    if(channel < 1 || channel > 8) return; // Validación de canal

    // Escribir en el registro CHnSET para desactivar el canal
    // Registro CHnSET = 0x05 + (channel - 1)
    uint8_t reg = 0x05 + (channel - 1);
    uint8_t value = 0x81; // Power down = 1, Gain = 24, Input shorted

    ADS1299_WREG(reg, value);
}


