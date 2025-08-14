#include "ADS1299_Driver.h"
#include "fsl_gpio.h"
#include "fsl_iocon.h"
#include "fsl_spi.h"
#include "fsl_clock.h"
#include "board.h"

static uint8_t numChannels;
static uint8_t defaultChannelSettings[6];
static uint8_t channelSettings[8][6];
static bool useInBias[8];
static bool useSRB2[8];

#define ADS1299_SPI_BASE     SPI0
#define ADS1299_SPI_BAUDRATE 1000000U
#define ADS1299_SPI_SSEL     kSPI_Ssel0Assert

static inline void delay_ms(uint32_t ms)
{
    SDK_DelayAtLeastUs(ms * 1000U, SystemCoreClock);
}

static inline void delay_us(uint32_t us)
{
    SDK_DelayAtLeastUs(us, SystemCoreClock);
}

static void ADS1299_InitPins(void)
{
    gpio_pin_config_t input_config = {
        .pinDirection = kGPIO_DigitalInput,
        .outputLogic = 0U,
    };

    gpio_pin_config_t output_high = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 1U,
    };

    gpio_pin_config_t output_low = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U,
    };

    GPIO_PortInit(GPIO, ADS_DRDY_PORT);
    GPIO_PortInit(GPIO, ADS_RST_PORT);
    GPIO_PortInit(GPIO, ADS_START_PORT);
    GPIO_PortInit(GPIO, BOARD_ADS_PORT);

    GPIO_PinInit(GPIO, BOARD_ADS_PORT, BOARD_ADS_PIN, &output_high);
    GPIO_PinInit(GPIO, ADS_RST_PORT, ADS_RST_PIN, &output_high);
    GPIO_PinInit(GPIO, ADS_START_PORT, ADS_START_PIN, &output_low);
    GPIO_PinInit(GPIO, ADS_DRDY_PORT, ADS_DRDY_PIN, &input_config);
}

static void ADS1299_InitSPI(void)
{
    spi_master_config_t spiConfig;
    SPI_MasterGetDefaultConfig(&spiConfig);
    spiConfig.baudRate_Bps = ADS1299_SPI_BAUDRATE;
    spiConfig.sselNumber = ADS1299_SPI_SSEL;
    SPI_MasterInit(ADS1299_SPI_BASE, &spiConfig, CLOCK_GetFreq(kCLOCK_MainClk));
}

static inline void ADS1299_csLow(void)
{
    GPIO_PinWrite(GPIO, BOARD_ADS_PORT, BOARD_ADS_PIN, 0U);
}

static inline void ADS1299_csHigh(void)
{
    GPIO_PinWrite(GPIO, BOARD_ADS_PORT, BOARD_ADS_PIN, 1U);
}

static uint8_t ADS1299_xfer(uint8_t data)
{
    uint8_t rx = 0U;
    spi_transfer_t xfer = {0};
    xfer.txData = &data;
    xfer.rxData = &rx;
    xfer.dataSize = 1;
    xfer.configFlags = kSPI_EndOfTransfer;
    SPI_MasterTransferBlocking(ADS1299_SPI_BASE, &xfer);
    return rx;
}

static void ADS1299_SDATAC(void)
{
    ADS1299_csLow();
    ADS1299_xfer(_SDATAC);
    ADS1299_csHigh();
    delay_us(3);
}

void ADS1299_Reset(void)
{
    ADS1299_csLow();
    ADS1299_xfer(_RESET);
    ADS1299_csHigh();
    delay_us(12);
    ADS1299_SDATAC();
}

void ADS1299_WriteDefaultChannelSettings(void)
{
    for (uint8_t ch = 1; ch <= numChannels; ++ch)
    {
        uint8_t value = 0;
        value |= (channelSettings[ch - 1][POWER_DOWN] & 0x01U) << 7;
        value |= channelSettings[ch - 1][GAIN_SET] & 0x70U;
        if (channelSettings[ch - 1][SRB2_SET] == YES)
        {
            value |= 0x08U;
        }
        value |= channelSettings[ch - 1][INPUT_TYPE_SET] & 0x07U;

        ADS1299_WriteRegister(CH1SET + (ch - 1), value);
    }
}

void ADS1299_Init(void)
{
    ADS1299_InitPins();
    ADS1299_InitSPI();

    delay_ms(50);

    GPIO_PinWrite(GPIO, ADS_RST_PORT, ADS_RST_PIN, 0U);
    delay_us(4);
    GPIO_PinWrite(GPIO, ADS_RST_PORT, ADS_RST_PIN, 1U);
    delay_us(20);

    ADS1299_Reset();
    delay_ms(20);

    numChannels = 8;
    defaultChannelSettings[POWER_DOWN] = NO;
    defaultChannelSettings[GAIN_SET] = ADS_GAIN24;
    defaultChannelSettings[INPUT_TYPE_SET] = ADSINPUT_NORMAL;
    defaultChannelSettings[BIAS_SET] = NO;
    defaultChannelSettings[SRB2_SET] = NO;
    defaultChannelSettings[SRB1_SET] = NO;

    for (uint8_t i = 0; i < numChannels; ++i)
    {
        for (uint8_t j = 0; j < 6; ++j)
        {
            channelSettings[i][j] = defaultChannelSettings[j];
        }
        useInBias[i] = true;
        useSRB2[i] = true;
    }

    ADS1299_WriteDefaultChannelSettings();
    ADS1299_WriteRegister(CONFIG1, 0x90 | SAMPLE_RATE_250HZ);
    ADS1299_WriteRegister(LOFF, 0x02);
}

void ADS1299_Start(void)
{
    ADS1299_csLow();
    ADS1299_xfer(_START);
    ADS1299_csHigh();
}

void ADS1299_Stop(void)
{
    ADS1299_csLow();
    ADS1299_xfer(_STOP);
    ADS1299_csHigh();
}

uint8_t ADS1299_ReadRegister(uint8_t reg)
{
    uint8_t value;
    ADS1299_csLow();
    ADS1299_xfer(_RREG | reg);
    ADS1299_xfer(0x00);		// Quantity of registers to read minus one.
    value = ADS1299_xfer(0x00);
    delay_us(10); 			// After the serial communication is finished, always wait four or more tCLK cycles before taking CS high
    ADS1299_csHigh();
    return value;
}

void ADS1299_WriteRegister(uint8_t reg, uint8_t value)
{
    ADS1299_csLow();
    ADS1299_xfer(_WREG | reg);
    ADS1299_xfer(0x00);
    ADS1299_xfer(value);
    delay_us(10);
    ADS1299_csHigh();
}

void ADS1299_SetSampleRate(uint8_t rate)
{
    uint8_t config1 = ADS1299_ReadRegister(CONFIG1);
    config1 = (config1 & 0xF8U) | (rate & 0x07U);
    ADS1299_WriteRegister(CONFIG1, config1);
}

bool ADS1299_IsDataReady(void)
{
    return GPIO_PinRead(GPIO, ADS_DRDY_PORT, ADS_DRDY_PIN) == 0U;
}

void ADS1299_ReadData(int32_t *channelData, uint32_t *status)
{
    uint8_t buffer[27];
    ADS1299_csLow();
    for (uint8_t i = 0; i < 27; ++i)
    {
        buffer[i] = ADS1299_xfer(0x00);
    }
    ADS1299_csHigh();

    if (status != NULL)
    {
        *status = ((uint32_t)buffer[0] << 16) |
                  ((uint32_t)buffer[1] << 8) |
                  buffer[2];
    }

    for (uint8_t ch = 0; ch < 8; ++ch)
    {
        uint32_t raw = ((uint32_t)buffer[3 + ch * 3] << 16) |
                       ((uint32_t)buffer[4 + ch * 3] << 8) |
                       buffer[5 + ch * 3];
        if (raw & 0x800000U)
        {
            raw |= 0xFF000000U;
        }
        channelData[ch] = (int32_t)raw;
    }
}

uint8_t ADS1299_GetDeviceID(void)
{
    return ADS1299_ReadRegister(ID_REG);
}

void ADS1299_SetChannelGain(uint8_t channel, uint8_t gain)
{
    if (channel < 1U || channel > numChannels)
    {
        return;
    }
    channelSettings[channel - 1U][GAIN_SET] = gain & 0x70U;
    uint8_t value = 0U;
    value |= (channelSettings[channel - 1U][POWER_DOWN] & 0x01U) << 7;
    value |= channelSettings[channel - 1U][GAIN_SET];
    if (channelSettings[channel - 1U][SRB2_SET] == YES)
    {
        value |= 0x08U;
    }
    value |= channelSettings[channel - 1U][INPUT_TYPE_SET] & 0x07U;
    ADS1299_WriteRegister(CH1SET + (channel - 1U), value);
}

void ADS1299_DeactivateChannel(uint8_t channel)
{
    if (channel < 1U || channel > numChannels)
    {
        return;
    }
    channelSettings[channel - 1U][POWER_DOWN] = YES;
    ADS1299_WriteRegister(CH1SET + (channel - 1U), 0x81U);
}

void ADS1299_ActivateChannel(uint8_t channel)
{
    if (channel < 1U || channel > numChannels)
    {
        return;
    }
    channelSettings[channel - 1U][POWER_DOWN] = NO;
    uint8_t value = 0U;
    value |= (channelSettings[channel - 1U][POWER_DOWN] & 0x01U) << 7;
    value |= channelSettings[channel - 1U][GAIN_SET] & 0x70U;
    if (channelSettings[channel - 1U][SRB2_SET] == YES)
    {
        value |= 0x08U;
    }
    value |= channelSettings[channel - 1U][INPUT_TYPE_SET] & 0x07U;
    ADS1299_WriteRegister(CH1SET + (channel - 1U), value);
}
