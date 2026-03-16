#include "ADS1299_Driver.h"
#include "fsl_gpio.h"
#include "fsl_iocon.h"
#include "fsl_spi.h"
#include "fsl_clock.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_usart.h"


static uint8_t numChannels = 8;
static uint8_t defaultChannelSettings[6];
static uint8_t channelSettings[8][6];
static bool useInBias[8];
static bool useSRB2[8];
static volatile int32_t channelData[8];
static uint32_t status;

#define ADS1299_SPI_BASE     SPI0
#define ADS1299_SPI_BAUDRATE 2000000U
#define ADS1299_SPI_SSEL     kSPI_SselDeAssertAll


static inline void delay_ms(uint32_t ms)
{
    SDK_DelayAtLeastUs(ms * 1000U, SystemCoreClock);
}

static inline void initializeDefaultChannelSettings(void)
{
    defaultChannelSettings[POWER_DOWN] = NO;
    defaultChannelSettings[GAIN_SET] = ADS_GAIN24;
    defaultChannelSettings[INPUT_TYPE_SET] = ADSINPUT_NORMAL;
    defaultChannelSettings[BIAS_SET] = YES;
    defaultChannelSettings[SRB2_SET] = NO;
    defaultChannelSettings[SRB1_SET] = NO;
}

static inline void updateChannelSettingsToDefault(void)
{
    for (uint8_t i = 0; i < numChannels; ++i)
    {
        for (uint8_t j = 0; j < 6; ++j)
        {
            channelSettings[i][j] = defaultChannelSettings[j];
        }
        useInBias[i] = true;
        useSRB2[i] = true;
    }
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
    /*GPIO_PortInit(GPIO, ADS_RST_PORT);
    GPIO_PortInit(GPIO, ADS_START_PORT);
    GPIO_PortInit(GPIO, BOARD_ADS_PORT);*/

    GPIO_PinInit(GPIO, BOARD_ADS_PORT, BOARD_ADS_PIN, &output_high);
    GPIO_PinInit(GPIO, ADS_RST_PORT, ADS_RST_PIN, &output_high);
    GPIO_PinInit(GPIO, PWDN_ADS_PORT, PWDN_ADS_PIN, &output_high);
    GPIO_PinInit(GPIO, ADS_START_PORT, ADS_START_PIN, &output_low);

    GPIO_PinInit(GPIO, ADS_DRDY_PORT, ADS_DRDY_PIN, &input_config);
}

static void ADS1299_InitSPI(void)
{
    spi_master_config_t userConfig = {0};
    uint32_t srcFreq               = 0U;
    SPI_MasterGetDefaultConfig(&userConfig);
    userConfig.baudRate_Bps           = ADS1299_SPI_BAUDRATE;
    userConfig.sselNumber             = ADS1299_SPI_SSEL; //EXAMPLE_SPI_MASTER_SSEL;
    userConfig.clockPhase             = kSPI_ClockPhaseSecondEdge;
    srcFreq                           = CLOCK_GetFreq(kCLOCK_MainClk);
    SPI_MasterInit(ADS1299_SPI_BASE, &userConfig, srcFreq);
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
    xfer.dataSize = sizeof(data);
    xfer.configFlags = 0;//kSPI_EndOfTransfer;

    SPI_MasterTransferBlocking(ADS1299_SPI_BASE, &xfer);

    return rx;
}

static void ADS1299_SDATAC(void)
{
    ADS1299_csLow();
    ADS1299_xfer(_SDATAC);
    delay_us(10);
    ADS1299_csHigh();
    delay_us(10);
}


static void ADS1299_RDATAC(void)
{
    ADS1299_csLow();
    ADS1299_xfer(_RDATAC);
    delay_us(10);
    ADS1299_csHigh();
    delay_us(10);
}


void ADS1299_Reset(void)
{
    ADS1299_csLow();
    ADS1299_xfer(_RESET);
    delay_us(10);
    ADS1299_csHigh();
    delay_us(12);
    ADS1299_SDATAC();

}

void ADS1299_WriteChannelSettings(void)
{
    for (uint8_t ch = 1; ch <= numChannels; ++ch)
    {
    	ADS1299_WriteOneChannelSettings(ch);
    }
}

void ADS1299_WriteDefaultChannelSettings(void)
{
    updateChannelSettingsToDefault();
    ADS1299_WriteChannelSettings();
}
/*
void ADS1299_WriteOneChannelSettings(uint8_t channel)
{
    uint8_t value = 0;
    value |= (channelSettings[channel - 1][POWER_DOWN] & 0x01U) << 7;
    value |= channelSettings[channel - 1][GAIN_SET] & 0x70U;

    if (channelSettings[channel - 1][SRB2_SET] == YES)
		{
			value |= 0x08U;
		}

    value |= channelSettings[channel - 1][INPUT_TYPE_SET] & 0x07U;

    ADS1299_WriteRegister(CH1SET + (channel - 1), value);

	// add or remove from inclusion in BIAS generation
    uint8_t setting = 0x00;

	setting = ADS1299_ReadRegister(BIAS_SENSP);       //get the current P bias settings

	if(channelSettings[channel-1][BIAS_SET] == YES)
	{
		useInBias[channel-1] = true;
		bitSet(setting,channel-1);    //set this channel's bit to add it to the bias generation
	}
	else
	{
		useInBias[channel-1] = false;
		bitClear(setting,channel-1);  // clear this channel's bit to remove from bias generation
	}

	ADS1299_WriteRegister(BIAS_SENSP,setting); //send the modified byte back to the ADS
	delay_ms(1);

	setting = ADS1299_ReadRegister(BIAS_SENSN);       //get the current N bias settings

	if(channelSettings[channel-1][BIAS_SET] == YES && channelSettings[channel-1][SRB1_SET] == NO)
	{
		bitSet(setting,channel-1);    //set this channel's bit to add it to the bias generation
	}
	else
	{
		bitClear(setting,channel-1);  // clear this channel's bit to remove from bias generation
	}

	ADS1299_WriteRegister(BIAS_SENSN,setting); //send the modified byte back to the ADS
	delay_ms(1);

	if(channelSettings[channel-1][SRB1_SET] == YES)
	{
		for(uint8_t i=0; i<numChannels ; i++)
		{
			channelSettings[i][SRB1_SET] = YES;
		}
		setting = 0x20;     // close SRB1 switch
	}
	else
	{
		for(uint8_t i=0; i<numChannels ; i++)
		{
			channelSettings[i][SRB1_SET] = NO;
		}
		setting = 0x00;     // open SRB1 switch
	}

	ADS1299_WriteRegister(MISC1,setting);
}
*/
/*void ADS1299_WriteOneChannelSettings(uint8_t channel)
{
    uint8_t value = 0;

    // --- 1. Configuración del registro CHnSET (Gain, Input Type, SRB2, PowerDown) ---
    value |= (channelSettings[channel - 1][POWER_DOWN] & 0x01U) << 7;
    value |= channelSettings[channel - 1][GAIN_SET] & 0x70U;

    if (channelSettings[channel - 1][SRB2_SET] == YES)
    {
        value |= 0x08U;
    }

    value |= channelSettings[channel - 1][INPUT_TYPE_SET] & 0x07U;

    ADS1299_WriteRegister(CH1SET + (channel - 1), value);

    // --- 2. Configuración BIAS SENSP (Entradas Positivas) ---
    // Aquí siempre habilitamos si el usuario quiere Bias, ya que son los electrodos activos.
    uint8_t setting = 0x00;

    setting = ADS1299_ReadRegister(BIAS_SENSP);        // get the current P bias settings

    if(channelSettings[channel-1][BIAS_SET] == YES)
    {
        useInBias[channel-1] = true;
        bitSet(setting, channel-1);    // set this channel's bit to add it to the bias generation
    }
    else
    {
        useInBias[channel-1] = false;
        bitClear(setting, channel-1);  // clear this channel's bit to remove from bias generation
    }

    ADS1299_WriteRegister(BIAS_SENSP, setting); // send the modified byte back to the ADS
    delay_ms(1);

    // --- 3. Configuración BIAS SENSN (Entradas Negativas) ---
    // CORRECCIÓN: Si SRB1 está activo (Monopolar), NO debemos habilitar el bit en SENSN
    // para evitar retroalimentar la referencia al lazo de Bias.

    setting = ADS1299_ReadRegister(BIAS_SENSN);        // get the current N bias settings

    if((channelSettings[channel-1][BIAS_SET] == YES) && (channelSettings[channel-1][SRB1_SET] == NO))
    {
        // Solo agregamos la entrada N al Bias si NO estamos usando SRB1 (es decir, solo en modo Bipolar)
        bitSet(setting, channel-1);
    }
    else
    {
        // Si no hay Bias, O SI ESTAMOS USANDO SRB1, limpiamos el bit.
        bitClear(setting, channel-1);
    }

    ADS1299_WriteRegister(BIAS_SENSN, setting); // send the modified byte back to the ADS
    delay_ms(1);

    // --- 4. Configuración Global SRB1 (MISC1) ---
    if(channelSettings[channel-1][SRB1_SET] == YES)
    {
        for(uint8_t i=0; i<numChannels ; i++)
        {
            channelSettings[i][SRB1_SET] = YES;
        }
        setting = 0x20;     // close SRB1 switch (Connect inputs N to SRB1 pin)
    }
    else
    {
        for(uint8_t i=0; i<numChannels ; i++)
        {
            channelSettings[i][SRB1_SET] = NO;
        }
        setting = 0x00;     // open SRB1 switch
    }

    ADS1299_WriteRegister(MISC1, setting);
}*/









void ADS1299_WriteOneChannelSettings(uint8_t channel)
{
    uint8_t value = 0;

    // --- 1. Configuración CHnSET (Igual que tu código) ---
    value |= (channelSettings[channel - 1][POWER_DOWN] & 0x01U) << 7;
    value |= channelSettings[channel - 1][GAIN_SET] & 0x70U;

    if (channelSettings[channel - 1][SRB2_SET] == YES)
    {
        value |= 0x08U; // Cierra switch SRB2 (P inputs)
    }

    value |= channelSettings[channel - 1][INPUT_TYPE_SET] & 0x07U;
    ADS1299_WriteRegister(CH1SET + (channel - 1), value);

    // --- 2. Configuración BIAS SENSP (Igual que tu código) ---
    uint8_t setting_p = ADS1299_ReadRegister(BIAS_SENSP);
    if(channelSettings[channel-1][BIAS_SET] == YES)
    {
        useInBias[channel-1] = true;
        bitSet(setting_p, channel-1);
    }
    else
    {
        useInBias[channel-1] = false;
        bitClear(setting_p, channel-1);
    }
    ADS1299_WriteRegister(BIAS_SENSP, setting_p);
    delay_ms(1);

    // --- 3. Detección de Cambio Global SRB1 ---
    // Leemos el estado actual REAL del registro MISC1 para ver si va a cambiar
    uint8_t current_misc1 = ADS1299_ReadRegister(MISC1);
    bool srb1_was_active = (current_misc1 & 0x20);
    bool srb1_will_be_active = (channelSettings[channel-1][SRB1_SET] == YES);
    bool global_srb1_changed = (srb1_was_active != srb1_will_be_active);

    // --- 4. Actualización Global de Variables SRB1 ---
    // Forzamos la coherencia en el array de configuración
    uint8_t new_misc1_val = 0x00;
    if(srb1_will_be_active)
    {
        for(uint8_t i=0; i<numChannels ; i++) channelSettings[i][SRB1_SET] = YES;
        new_misc1_val = 0x20;
    }
    else
    {
        for(uint8_t i=0; i<numChannels ; i++) channelSettings[i][SRB1_SET] = NO;
        new_misc1_val = 0x00;
    }

    // Escribimos MISC1 primero
    if(global_srb1_changed)
    {
        ADS1299_WriteRegister(MISC1, new_misc1_val);
        delay_ms(1);
    }

    // --- 5. Configuración BIAS SENSN (Con protección global) ---

    if (global_srb1_changed)
    {
        // CASO A: Si SRB1 cambió globalmente, debemos refrescar SENSN de TODOS los canales
        // para evitar que un canal "antiguo" inyecte SRB1 al Bias.
        uint8_t bias_sensn_reg = 0x00; // Recalculamos todo el byte desde cero

        for(uint8_t i=0; i<numChannels; i++)
        {
            // La misma lógica: Solo incluir en Bias N si BIAS=YES y SRB1=NO
            if((channelSettings[i][BIAS_SET] == YES) && (channelSettings[i][SRB1_SET] == NO))
            {
                bitSet(bias_sensn_reg, i);
            }
            // Si SRB1 es YES (que lo es para todos si srb1_will_be_active es true), esto queda en 0. Correcto.
        }
        ADS1299_WriteRegister(BIAS_SENSN, bias_sensn_reg);
    }
    else
    {
        // CASO B: Si SRB1 no cambió globalmente, solo actualizamos el bit de ESTE canal
        // (Optimización para no escribir en todos los canales si no es necesario)
        uint8_t setting_n = ADS1299_ReadRegister(BIAS_SENSN);

        if((channelSettings[channel-1][BIAS_SET] == YES) && (channelSettings[channel-1][SRB1_SET] == NO))
        {
            bitSet(setting_n, channel-1);
        }
        else
        {
            bitClear(setting_n, channel-1);
        }
        ADS1299_WriteRegister(BIAS_SENSN, setting_n);
    }

    delay_ms(1);
}
































void ADS1299_writeNewChannelSettings(uint8_t* newChannelSettings)
{
	uint8_t channelSelected = newChannelSettings[0];

	for(uint8_t i =0 ; i < 6 ; i++)
	{
		channelSettings[channelSelected-1][i] = newChannelSettings[i+1] ;
	}
	channelSettings[channelSelected-1][1] = newChannelSettings[2]<<4;
}

void ADS1299_Init(void)
{
    ADS1299_InitSPI();
    ADS1299_InitPins();

    delay_ms(50);

    GPIO_PinWrite(GPIO, ADS_RST_PORT, ADS_RST_PIN, 0U);
    delay_us(10);
    GPIO_PinWrite(GPIO, ADS_RST_PORT, ADS_RST_PIN, 1U);
    delay_us(20);

    ADS1299_Reset();
    delay_ms(20);

    initializeDefaultChannelSettings();
    ADS1299_WriteDefaultChannelSettings();

    ADS1299_WriteRegister(CONFIG1, 0xD0 | SAMPLE_RATE_250HZ);
    ADS1299_WriteRegister(LOFF, 0x02);
    delay_us(10);

    ADS1299_WriteRegister(CONFIG3,0b11101100);
    delay_us(10);
}

void ADS1299_StartStreaming(void)
{
	ADS1299_RDATAC();
	delay_ms(1);
	ADS1299_Start();
	delay_ms(1);
}

void ADS1299_Start(void)
{
    ADS1299_csLow();
    ADS1299_xfer(_START);
    delay_us(10);
    ADS1299_csHigh();
}

void ADS1299_Stop(void)
{
    ADS1299_csLow();
    ADS1299_xfer(_STOP);
    delay_us(10);
    ADS1299_csHigh();
}

uint8_t ADS1299_ReadRegister(uint8_t reg)
{
    uint8_t value;
    ADS1299_csLow();
    delay_us(5);
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

void ADS1299_UpdateChannelData(void)
{
	while(!ADS1299_IsDataReady()){}
	ADS1299_ReadData(&channelData[0], &status);
}

void ADS1299_copyLatest(int32_t out[8])
{
    memcpy(out, (const void*)channelData, sizeof(channelData));
}


void ADS1299_ReadData(int32_t *channelData, uint32_t *status)
{
    uint8_t buffer[27];
    ADS1299_csLow();
    for (uint8_t i = 0; i < 27; ++i)
    {
        buffer[i] = ADS1299_xfer(0x00);
    }
    delay_us(10);
    ADS1299_csHigh();

    if (status != NULL)
    {
        *status = ((uint32_t)buffer[0] << 16) |
                  ((uint32_t)buffer[1] << 8) |
                  buffer[2];
    }

    for (uint8_t ch = 0; ch < 8; ++ch)
    {
        int32_t raw = ((uint32_t)buffer[3 + ch * 3] << 16) |
                       ((uint32_t)buffer[4 + ch * 3] << 8) |
                       buffer[5 + ch * 3];
        // Completa con 0s o 1s para complemento a dos
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

void ADS1299_changeInputType(uint8_t inputCode)
{
	uint8_t i = 0; //loop counter
	for(i=0; i<numChannels; i++)
	{
		channelSettings[i][INPUT_TYPE_SET] = inputCode;
	}

	ADS1299_WriteDefaultChannelSettings();
}

void ADS1299_configureInternalTestSignal(uint8_t amplitudeCode, uint8_t freqCode)
{
	uint8_t setting;

	if(amplitudeCode == ADSTESTSIG_NOCHANGE)
	{
		amplitudeCode = ADS1299_ReadRegister(CONFIG2) & (0b00000100);
	}

	if(freqCode == ADSTESTSIG_NOCHANGE)
	{
		freqCode = ADS1299_ReadRegister(CONFIG2) & (0b00000011);
	}

	freqCode &= 0b00000011;  		//only the last two bits are used
	amplitudeCode &= 0b00000100;  	//only this bit is used
	setting = 0b11010000 | freqCode | amplitudeCode;  //compose the code

	ADS1299_WriteRegister(CONFIG2, setting);
	delay_ms(1);
}


bool ADS1299_TestChannelRegisters(void)
{
    uint8_t ok = 1U;
    const uint8_t n = numChannels;     // numChannels ya lo usás en tu código (1..8)
    uint8_t original[8];

    // 1) Salir de RDATAC para poder usar RREG/WREG sin interferencias
    ADS1299_SDATAC();

    // 2) Leer y guardar originales
    for (uint8_t i = 0; i < n; i++)
    {
        original[i] = ADS1299_ReadRegister((uint8_t)(CH1SET + i));
    }

    // 3) Patrón de prueba #1: canal activo, ganancia 24, entradas cortocircuitadas (mux=SHORTED)
    //    Bits CHnSET: [7]=PD, [6:4]=GAIN, [3]=SRB2, [2:0]=MUX
    const uint8_t pat1 = (0U<<7) | (ADS_GAIN24) | (0U<<3) | (ADSINPUT_SHORTED & 0x07U);

    for (uint8_t i = 0; i < n; i++)
    {
        ADS1299_WriteRegister((uint8_t)(CH1SET + i), pat1);
        uint8_t rb = ADS1299_ReadRegister((uint8_t)(CH1SET + i));
        if (rb != pat1) { ok = 0U; }
    }

    // 4) Patrón de prueba #2: canal apagado (PD=1), ganancia 6, entrada normal (mux=NORMAL)
    const uint8_t pat2 = (1U<<7) | (ADS_GAIN06) | (0U<<3) | (ADSINPUT_NORMAL & 0x07U);

    for (uint8_t i = 0; i < n; i++)
    {
        ADS1299_WriteRegister((uint8_t)(CH1SET + i), pat2);
        uint8_t rb = ADS1299_ReadRegister((uint8_t)(CH1SET + i));
        if (rb != pat2) { ok = 0U; }
    }

    // 5) Restaurar originales
    for (uint8_t i = 0; i < n; i++)
    {
        ADS1299_WriteRegister((uint8_t)(CH1SET + i), original[i]);
        uint8_t rb = ADS1299_ReadRegister((uint8_t)(CH1SET + i));
        if (rb != original[i]) { ok = 0U; }
    }

    // 6) Volver a RDATAC si tu flujo normal lo usa
    //ADS1299_SendCommand(_RDATAC);

    return (ok != 0U);
}


