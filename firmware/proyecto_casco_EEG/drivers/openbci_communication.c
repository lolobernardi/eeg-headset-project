/*
 * openbci_communication.c
 *
 *  Created on: 27 ago. 2025
 *      Author: Lorenzo Bernardi y Francisco Dodera
 */

// INCLUDES
#include "openbci_communication.h"
#include "Bluetooth_Driver.h"
#include "ADS1299_Driver.h"
#include "ADS1299_Parameters.h"

// VARIABLES

static uint8_t Header = 0xA0;
static uint8_t Footer = 0xC0;
static uint8_t zero = 0x00;
static uint8_t sampleCnt = 0;

uint8_t newChannelSettings[6] = {0};

/**
 * @brief Flag to indicate if the ADS1299 is acquiring data or not
 */
bool isRunning = false;
/**
 * @brief Flag to indicate if channel settings commands are being received
 */
bool get_channel_settings = false;
/**
 * @brief Counter used to retrieve channel settings from serial port
 */
uint8_t channel_settings_counter = 0;
/**
 * @brief Keep track of what channel we are loading settings for
 */
uint8_t channel_selected;

uint8_t dataReceived[7] = {0};

uint8_t counter = 0;
/**
 * @brief Mode of use: 8 channels
 */
uint8_t cur_outputType = OUTPUT_8_CHAN;
/**
 * @brief Flag to indicate if lead off settings commands are being received
 */
bool get_leadoff_settings = false;
/**
 * @brief Counter used to retrieve lead off settings from serial port
 */
uint8_t leadoff_settings_counter = 0;


volatile uint32_t g_millis = 0;

void SysTick_Handler(void)
{
    g_millis++;
}

void init_millis(void)
{
    // Carga para 1 ms: CoreClock / 1000
    uint32_t sysClock = CLOCK_GetFreq(kCLOCK_CoreSysClk);
    SysTick_Config(sysClock / 10000U);
}

uint32_t millis(void)
{
    return g_millis;
}

// DELAY
static inline void delay_ms(uint32_t ms)
{
    SDK_DelayAtLeastUs(ms * 1000U, SystemCoreClock);
}
//FUNCTIONS
/**
 * @brief Start the system from scratch, send initial message and get the board ready to get commands
 */
void InitCommunication()
{
	if(!isRunning)
	{
		BoardPresentation();
		SendEndOfTransmission();
	}
}
/**
* @description: This is a function that can be called multiple times, this is
*                 what we refer to as a `soft reset`. You will hear/see this
*                 many times.
* @author: AJ Keller (@pushtheworldllc)
*/
void BoardPresentation(void) {

	char vstring [2];
	HC05_SendString("Poncho de Biopotenciales V1.1 8 canales\r\n"); //modificar al nombre de nuestro proyecto después de confirmar que funciona
    ///HC05_SendString("ID del ADS1299: 0x");
    //itoa(ADS1299_GetDeviceID(),vstring,16); // convert to hexadecimal
    //HC05_SendString(vstring);
    //HC05_SendString("\r\n");

	HC05_SendString("ID del ADS1299: 0x25\r\n");
    HC05_SendString("Firmware: v1.0.0\r\n");
    //PrintMenu();

    SendEndOfTransmission();
}

/**
 * @brief Function for receiving new commands
 */
void EventSerial(uint8_t data)
{
	if(data == 'x' || get_channel_settings)
	{
		if(data != 'x' && data != 'X')
		{
			dataReceived[counter] = data - '0';
			counter++;
		}

		get_channel_settings = true;

		if(data == 'X')
		{
			WriteChannelSettings(&dataReceived[0]);
			get_channel_settings = false;
			counter = 0;
		}
	}else if(get_leadoff_settings) // if we just got a 'z' expect lead-off setting parameters
	{
		//LoadLeadOffSettings(data); //go get lead-off settings parameters
	}else
	{
		HandleOpenBCICommand(data); // decode the command
	}


	/*if(counter == 15){
		get_channel_settings = true;
	}
	if(get_channel_settings)
	{
		counter = 0;
		get_channel_settings = false;
	}*/
	/*if(get_channel_settings) // if we just got an 'x' expect channel setting parameters
	{
		dataReceived[counter] = data;
		counter++;

		if(counter == 7){
			get_channel_settings = false;
			counter = 0;
		}
		//LoadChannelSettings(data); //go get channel settings parameters
	}
	else if(get_leadoff_settings) // if we just got a 'z' expect lead-off setting parameters
	{
		//LoadLeadOffSettings(data); //go get lead-off settings parameters
	}

	else
	{
		HandleOpenBCICommand(data); // decode the command
	}*/

}

/**
 * @brief Function for receiving new commands
 * @param command Character sent from OPENBCI GUI and received by HC05 bluetooth module
 */
void HandleOpenBCICommand(uint8_t command)
{
	switch (command)
		{
		/* Number ASCII characters turn the respective channels [1-8] off */
			case '1':
					ChangeChannelState(1, DEACTIVATE);
					break;
			case '2':
					ChangeChannelState(2, DEACTIVATE);
					break;
			case '3':
					ChangeChannelState(3, DEACTIVATE);
					break;
			case '4':
					ChangeChannelState(4, DEACTIVATE);
					break;
			case '5':
					ChangeChannelState(5, DEACTIVATE);
					break;
			case '6':
					ChangeChannelState(6, DEACTIVATE);
					break;
			case '7':
					ChangeChannelState(7, DEACTIVATE);
					break;
			case '8':
					ChangeChannelState(8, DEACTIVATE);
					break;

		/* Next ASCII characters turn the respective channels [1-8] on */
			case '!':
					ChangeChannelState(1, ACTIVATE);
					break;
			case '@':
					ChangeChannelState(2, ACTIVATE);
					break;
			case '#':
					ChangeChannelState(3, ACTIVATE);
					break;
			case '$':
					ChangeChannelState(4, ACTIVATE);
					break;
			case '%':
					ChangeChannelState(5, ACTIVATE);
					break;
			case '^':
					ChangeChannelState(6, ACTIVATE);
					break;
			case '&':
					ChangeChannelState(7, ACTIVATE);
					break;
			case '*':
					ChangeChannelState(8, ACTIVATE);
					break;

		/* Test signal control commands  */
			case '0': //Connect to internal GND (VDD - VSS)
					AllChannelsToTestCondition(ADSINPUT_SHORTED, ADSTESTSIG_NOCHANGE, ADSTESTSIG_NOCHANGE);
					break;
			case '-': //Connect to test signal 1xAmplitude, slow pulse
					AllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_1X, ADSTESTSIG_PULSE_SLOW);
					break;
			case '=': //Connect to test signal 1xAmplitude, fast pulse
					AllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_1X, ADSTESTSIG_PULSE_FAST);
					break;
			case 'p': //Connect to DC signal
					AllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_2X, ADSTESTSIG_DCSIG);
					break;
			case '[': //Connect to test signal 2xAmplitude, slow pulse
					AllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_2X, ADSTESTSIG_PULSE_SLOW);
					break;
			case ']': //Connect to test signal 2xAmplitude, fast pulse
					AllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_2X, ADSTESTSIG_PULSE_FAST);
					break;

		/* Channel Setting Commands  */

			case 'x': // enters Channel Settings mode
					if(!isRunning)
					{
						HC05_SendString("Ready to accept new channel settings\r\n");
					}
					//counter = 0;
					channel_settings_counter = 0;
					//get_channel_settings = true;

					 break;
			case 'X':
					if(!isRunning)
					{
						HC05_SendString("Updating channel settings\r\n");
					}
					/*ADS1299_writeNewChannelSettings(channel_selected, &newChannelSettings[0]);
					WriteChannelSettings(channel_selected); //Corroborar con Alejandro si vale la pena
					get_channel_settings = false;*/
					break;
			case 'd':
					if(!isRunning)
					{
						HC05_SendString("Updating channel settings to Default\r\n");
						SendEndOfTransmission();
					}
					SetChannelsToDefaultSettings();
					break;
			case 'D':
					//PrintDefaultChannelSettings();
					break;

			/* Change sampling frequency */
			case 'A':
					if(!isRunning)
					{
						HC05_SendString("Updating channels sampling rate to 2kHz\r\n");
					}
					//UpdateFrequency(SAMPLE_RATE_2kHZ);
					break;
			case 'S':
					if(!isRunning)
					{
						HC05_SendString("Updating channels sampling rate to 1kHz\r\n");
					}
					UpdateFrequency(SAMPLE_RATE_1kHZ);
					break;
			case 'F':
					if(!isRunning)
					{
						HC05_SendString("Updating channels sampling rate to 500Hz\r\n");
					}
					UpdateFrequency(SAMPLE_RATE_500HZ);
					break;
			case 'G':
					if(!isRunning)
					{
						HC05_SendString("Updating channels sampling rate to 250Hz\r\n");
					}
					UpdateFrequency(SAMPLE_RATE_250HZ);
					break;

			/* Lead-off impedance detection commands */
			case 'z':
					if(!isRunning)
					{
						HC05_SendString("Ready to accept new impedance detect settings\r\n");
					}
					leadoff_settings_counter = 0; //reset counter
					get_leadoff_settings = true;
					break;
			case 'Z':
					if(!isRunning)
					{
						HC05_SendString("Updating impedance detect settings\r\n");
					}
					//ChangeChannelLeadOffDetect(channel_selected);
					break;

			/* Stream data and filter commands */
			case 'b':
					if(!isRunning)
					{
						ADS1299_StartStreaming();
						isRunning = true;
					}
					break;
			case 's':
					StopRunning();
					break;
			case 'f':
				    //useFilters = true;
					break;
			case 'g':
				    //useFilters = false;
					break;

			/* Initialize and verify */
			case 'v':
				    InitCommunication(); //initialize ADS and read device IDs
					break;

			/* Query the ADS registers */
			case '?':
				    PrintRegisters();
					break;

			/* Query the menu of actions */
			case 'M':
					PrintMenu();
					break;

			/* Default */
			default:
					break;

	}
}

/*void LoadChannelSettings(uint8_t parameter)
{
	channel_selected
	if(channel_settings_counter == 0)// if it's the first byte in this channel's array, this byte is the channel number to set
	{
		channel_selected = parameter - '0'; //- 1; // we just got the channel to load settings into (shift number down for array usage)
		channel_settings_counter++;

		if(!is_running)
		{
			uint8_t number = channel_selected + 1 + '0';
			Drv_UART_Send((uint8_t*)("Load settings of channel: "),26);
			Drv_UART_Send(&number,1);
			Drv_UART_Send((uint8_t*)("\r\n"),2);
		}

	}
	else
	{
		//  setting bytes are in order: POWER_DOWN, GAIN_SET, INPUT_TYPE_SET, BIAS_SET, SRB2_SET, SRB1_SET

		parameter = parameter - '0'; //convert char to number

		if(channel_settings_counter-1 == GAIN_SET)
		{
			parameter <<= 4;
		}

		// Fill array with current channel settings defined in the Drv_ADS1299 library
		newChannelSettings[channel_settings_counter-1] = parameter;

		channel_settings_counter++;

		if(channel_settings_counter == 7) // 1 currentChannelToSet, plus 6 channelSetting parameters
		{
			get_channel_settings = false;
		}
	}
}
*/

/**
 * @brief Function for printing commands menu
 */
void PrintMenu(void){
	if(!isRunning)
		{
			HC05_SendString("\r\n");
			HC05_SendString("\r\n");
			HC05_SendString("------------------\r\n");
			HC05_SendString("  Command list:\r\n");
			HC05_SendString("------------------\r\n");
			HC05_SendString("\r\n");
			HC05_SendString("send 'b' to start data stream\r\n");
			HC05_SendString("send 's' to stop data stream\r\n");
			HC05_SendString("use 1,2,3,4,5,6,7,8 to turn OFF channels\r\n");
			HC05_SendString("use !,@,#,$,%,^,&,* to turn ON channels\r\n");
			HC05_SendString("send 'd' to set channels to default\r\n");
			HC05_SendString("send '?' to print all registers\r\n");
			HC05_SendString("send 'v' to initialize board\r\n");
			HC05_SendString("send 0,-,=,p,[,] to enable test signals\r\n");
			HC05_SendString("send 'z (CHANNEL, PCHAN, NCHAN) Z' to start impedance test (see User Manual)\r\n");
			HC05_SendString("send 'x (CHANNEL, POWER_DOWN, GAIN_SET, INPUT_TYPE_SET, BIAS_SET, SRB2_SET, SRB1_SET) X'  for channel setting (see User Manual)\r\n");
			HC05_SendString("send 'A' for 2kHz sample rate (only in 8chan)\r\n");
			HC05_SendString("send 'S' for 1kHz  sample rate\r\n");
			HC05_SendString("send 'F' for 500Hz sample rate\r\n");
			HC05_SendString("send 'G' for 250Hz sample rate\r\n");
		}
}

/**
 * @brief Stop data acquisition
 * @return HIGH if the device is running, LOW if not
 */
bool StopRunning()
{
	if(isRunning)
	{
		ADS1299_Stop(); //stop data acquisition
		isRunning = false;
	}

	return isRunning;
}

/**
 * @brief Start data acquisition
 * @param OUT_TYPE Define if quiet or how many channels are active:  OUTPUT_NOTHING, OUTPUT_8_CHAN, OUTPUT_16_CHAN
 * @return HIGH if the device is running, LOW if not
 */
bool StartRunning()
{
	if(!isRunning)
	{
		ADS1299_Start(); //start data acquisition
		isRunning = true;
	}

	return isRunning;
}

/**
 * @brief Activate or De-activate channel maintaining running state
 * @param channel Channel number
 * @param start Channel desired state: true (1) to activate channel, false (0) to de-activate channel
 */
void ChangeChannelState(uint8_t channel, uint8_t start)
{
	bool isRunning_when_called = isRunning; //check if when the function is called the program is already running

	StopRunning(); //must stop running to change channel settings

	if(start)
	{
		ADS1299_ActivateChannel(channel);
	}
	else if(!start)
	{
		ADS1299_DeactivateChannel(channel);
	}

	if(isRunning_when_called)
	{
		StartRunning(); //restart, if it was running before
	}
}

/**
 * @brief Set test inputs to all channels
 * @param inputCode Input types
 * @param amplitudCode Amplitude of the test signal: ADSTESTSIG_NOCHANGE, ADSTESTSIG_AMP_1X, ADSTESTSIG_AMP_2X
 * @param freqCode Frequence of the test signal: ADSTESTSIG_NOCHANGE, ADSTESTSIG_PULSE_SLOW, ADSTESTSIG_PULSE_FAST
 */
 void AllChannelsToTestCondition(uint8_t testInputCode, uint8_t amplitudeCode, uint8_t freqCode)
 {
	 bool isRunning_when_called = isRunning;

	 StopRunning(); //must stop running to change channel settings
	 delay_ms(10);

	 ADS1299_configureInternalTestSignal(amplitudeCode, freqCode);
	 ADS1299_changeInputType(testInputCode);

	 ADS1299_WriteChannelSettings();

	 if(isRunning_when_called == true)
	 {
		 StartRunning(); //restart if it was running before
	 }
 }

 /**
  * @brief Stop acquisition, set the channels and re-start acquisition
  * @param channel Channel number
  */

void WriteChannelSettings(uint8_t *channel)
 {
 	bool isRunning_when_called = isRunning;

 	StopRunning(); //must stop running to change channel settings

 	ADS1299_writeNewChannelSettings(channel);

 	ADS1299_WriteOneChannelSettings(channel[0]); //change the channel settings on ADS

 	if(isRunning_when_called)
 	{
 		StartRunning(); //restart, if it was running before
 	}
 }

 /**
  * @brief Set all channels to the default settings
  */
 void SetChannelsToDefaultSettings()
 {
 	bool isRunning_when_called = isRunning;

 	StopRunning(); //must stop running to change channel settings

 	ADS1299_WriteDefaultChannelSettings();

 	if(isRunning_when_called)
 	{
 		StartRunning(); //restart, if was running before
 	}
 }

/**
 * @brief Print the current state of all ADS1299 registers
 */

 void PrintRegisters()
{
	uint8_t i = 0; //loop counter
	uint8_t vstring [4];
	uint8_t regValue = 0x00;

	if(!isRunning)
	{
		HC05_SendString("\r\nBoard ADS Registers\r\n");

		for(i=0; i<24; i++) //send the 24 ADS1299 registers
		{
			regValue = ADS1299_ReadRegister(i);
			PrintRegisterName(i);
			HC05_SendString("0x");
			itoa(regValue,vstring,16); // convert to hexadecimal
			HC05_SendString(&vstring[0]);
			HC05_SendString("\r\n");
		}
		SendEndOfTransmission(); //end communication
		delay_ms(20);
	}
}

/**
* @brief To get and send by UART the name of a specific ADS1299 register
* @param regNumber Number of register in decimal (0-23)
*/
void PrintRegisterName(uint8_t reg)
{
	 switch(reg)
	 {
		case 0:
			HC05_SendString("ADS_ID: ");
			break;
		case 1:
			HC05_SendString(",CONFIG1: ");
			break;
		case 2:
			HC05_SendString(",CONFIG2: ");
			break;
		case 3:
			HC05_SendString(",CONFIG3: ");
			break;
		case 4:
			HC05_SendString(",LOFF: ");
			break;
		case 5:
			HC05_SendString(",CH1SET: ");
			break;
		case 6:
			HC05_SendString(",CH2SET: ");
			break;
		case 7:
			HC05_SendString(",CH3SET: ");
			break;
		case 8:
			HC05_SendString(",CH4SET: ");
			break;
		case 9:
			HC05_SendString(",CH5SET: ");
			break;
		case 10:
			HC05_SendString(",CH6SET: ");
			break;
		case 11:
			HC05_SendString(",CH7SET: ");
			break;
		case 12:
			HC05_SendString(",CH8SET: ");
			break;
		case 13:
			HC05_SendString(",BIAS_SENSP: ");
			break;
		case 14:
			HC05_SendString(",BIAS_SENSN: ");
			break;
		case 15:
			HC05_SendString(",LOFF_SENSP: ");
			break;
		case 16:
			HC05_SendString(",LOFF_SENSN: ");
			break;
		case 17:
			HC05_SendString(",LOFF_FLIP: ");
			break;
		case 18:
			HC05_SendString(",LOFF_STATP: ");
			break;
		case 19:
			HC05_SendString(",LOFF_STATN: ");
			break;
		case 20:
			HC05_SendString(",GPIO: ");
			break;
		case 21:
			HC05_SendString(",MISC1: ");
			break;
		case 22:
			HC05_SendString(",MISC2: ");
			break;
		case 23:
			HC05_SendString(",CONFIG4: ");
			break;
		default:
			break;
	 }
}

/**
 * @brief Send characters to shake hands with the controlling program
 */
void SendEndOfTransmission()
{
	HC05_SendString("$$$\r\n"); //send "$$$"
}


static inline void pack24be_from_s32(int32_t s, uint8_t out3[3])
{
    uint32_t u	= (uint32_t)s & 0x00FFFFFFu; // conserva los 24b de dos complementos
    out3[0]		= (uint8_t)((u >> 16) & 0xFF);
    out3[1]		= (uint8_t)((u >>  8) & 0xFF);
    out3[2]		= (uint8_t)( u        & 0xFF);
}

void SendChannelDataPruebaBT(void)
{
    if (!isRunning) return;

    ADS1299_UpdateChannelData();

    uint8_t  packet[33];
    int32_t  snap[8];
    ADS1299_copyLatest(snap);

    uint8_t idx = 0;
    packet[idx++] = Header;      // [0]
    packet[idx++] = sampleCnt++;      // [1] seq (0..255, wrap ok)

    // [2..25] 8 canales * 3 bytes (24B), big-endian por canal
    for (uint8_t ch = 0; ch < 8; ch++) {
        pack24be_from_s32(snap[ch], &packet[idx]);
        idx += 3;
    }

    // [26..29] tx_ms (uint32 LE): timestamp de TX del MCU para medir latencia
    const uint32_t tx_ms = millis();
    packet[26] = (uint8_t)(tx_ms & 0xFF);
    packet[27] = (uint8_t)((tx_ms >> 8) & 0xFF);
    packet[28] = (uint8_t)((tx_ms >> 16) & 0xFF);
    packet[29] = (uint8_t)((tx_ms >> 24) & 0xFF);

    // [30..31] reservados / status / checksum (si no usás, dejá 0)
    packet[30] = 0x00;
    packet[31] = 0x00;

    // [32] footer
    packet[32] = Footer;

    HC05_SendBytes(packet, sizeof(packet));
}


/**
 * @brief Send via BLUETOOTH the data acquired by the ADS
 */
 void SendChannelData()
 {
	if(!isRunning) return;

	ADS1299_UpdateChannelData();

	uint8_t packet[33];
	int32_t  snap[8];

	ADS1299_copyLatest(snap);

	const uint8_t *ptr_packet = &packet[0];

	packet[0] = Header;
	packet[1] = sampleCnt++;
	packet[32] = Footer;

	uint8_t idx = 2;
	for (uint8_t ch = 0; ch < 8; ch++)
	{
		pack24be_from_s32(snap[ch], &packet[idx]);
		idx += 3;
	}
	for(uint8_t i = 26 ; i<32 ;i++)
	{
		packet[i] = 0x00;
	}

	HC05_SendBytes(ptr_packet, 33);
 }

 /**
  * @brief Retrieve parameters and execute channel settings
  * @param parameter Setting parameter received as command: CHAN_NUM, POWER_DOWN, GAIN_SET, INPUT_TYPE_SET, BIAS_SET, SRB2_SET, SRB1_SET
  */
 /*
 void LoadChannelSettings(uint8_t parameter)
 {
 	if(channel_settings_counter == CHAN_NUM)// if it's the first byte in this channel's array, this byte is the channel number to set
 	{
 		channel_selected = parameter - '0';
 		channel_settings_counter++;
 		if(!isRunning)
 		{
 			HC05_SendString("Load settings of channel: ");
 			HC05_SendString(&(channel_selected + '0'));
 			HC05_SendString("\r\n");
 		}

 	}
 	else
 	{
 		//  setting bytes are in order: POWER_DOWN, GAIN_SET, INPUT_TYPE_SET, BIAS_SET, SRB2_SET, SRB1_SET

 		parameter = parameter - '0'; //convert char to number

 		if(channel_settings_counter == GAIN_SET)
 		{
 			parameter <<= 4; // ADS1299 registers for gain requires the gain data in 4 first bits
 		}

 		// Fill array with current channel settings defined in the Drv_ADS1299 library
 		channelSettings[channel_selected][channel_settings_counter-1] = parameter;

 		channel_settings_counter++;

 		if(channel_settings_counter == 7) // 1 channel number + 6 channelSetting parameters
 		{
 			get_channel_settings = false;
 		}
 	}
 }
*/
 /**
   * @brief Print the current Default Channel Settings
   */
/*
void PrintDefaultChannelSettings()
  {
 	 uint8_t defaultSettings[6];
 	 uint8_t i = 0; //loop counter
 	 bool isRunning_when_called = isRunning;

 	 //stop running
 	 StopRunning();

 	 ADS1299_getDefaultChannelSettings(defaultSettings); //get the array with default channel settings (AGREGAR)

 	 for(i=0; i<6; i++)
 	 {
 		 if(i==1)
 		 {
 			 defaultSettings[i] = (defaultSettings[i] >> 4) + 48; //48 is the decimal of the ASCCI '0'
 		 }
 		 else
 		 {
 			 defaultSettings[i] = defaultSettings[i] + 48; //48 is the decimal of the ASCCI '0'
 		 }

 		 HC05_SendString(&defaultSettings[i]);
 		 HC05_SendString("\r\n");
 	 }

 	 SendEndOfTransmission();
 	 delay_ms(10);

 	 //restart if it was running before
 	 if(isRunning_when_called == TRUE)
 	 {
 		 StartRunning(cur_outputType);
 	 }

  }
*/
 /**
  * @brief Update sampling frequency
  * @param freq Sampling frequency
  */
 void UpdateFrequency(uint8_t freq)
 {
 	bool isRunning_when_called = isRunning;

 	StopRunning(); //must stop running to change channel settings

 	ADS1299_SetSampleRate(freq);

 	if(isRunning_when_called)
 	{
 		StartRunning(); //restart, if it was running before
 	}
 }

 /**
  * @brief Change lead off detect settings and re-start device
  * @param channel Channel number
  */
 /*
 void ChangeChannelLeadOffDetect(uint8_t channel)
 {
	 bool isRunning_when_called = isRunning;

	 //must stop running to change channel settings
	 StopRunning();

	 ADS1299_changeOneChannelLeadOffDetect(channel); //AGREGAR --> NO ENTENDÍ MUY BIEN PARA QUE FUNCIONA

	 //restar if it was running before
	 if(isRunning_when_called == true)
	 {
		 StartRunning(cur_outputType);
	 }
 }
*/
 /**
 * @brief Retrieve parameters and execute channel settings
 * @param parameter Setting parameter received as command: PCHAN, NCHAN
 */
/*void LoadLeadOffSettings(uint8_t parameter)
{
	if(leadoff_settings_counter == 0)// if it's the first byte in this channel's array, this byte is the channel number to set
	{
		channel_selected = parameter - '0';
		leadoff_settings_counter++;

		if(!isRunning)
		{
			HC05_SendString("Load Lead-off settings of channel: ");
			HC05_SendString(&(channel_selected + '0'));
			HC05_SendString("\r\n");
		}

	}
	else
	{//  setting bytes are in order: PCHAN, NCHAN
		parameter = parameter - '0'; //convert char to number

		leadOffSettings[channel_selected][leadoff_settings_counter-1] = parameter; //CREO QUE HAY QUE AGREGAR ESA VARIABLE

		leadoff_settings_counter++;

		if(leadoff_settings_counter == 3) // 1 channel, plus 2 leadOff setting parameters
		{
			get_leadoff_settings = FALSE;
		}
	}
}
*/
