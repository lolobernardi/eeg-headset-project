/*
 * openbci_communication.h
 *
 *  Created on: 27 ago. 2025
 *      Author: Lorenzo Bernardi y Francisco Dodera
 */


#ifndef OPENBCI_COMMUNICATION_H_
#define OPENBCI_COMMUNICATION_H_

#include "board.h"
#include "app.h"
/** @defgroup Devices_Library
 *  @ingroup "Biopoteciales-LIRINS"
 *  Driver for controlling the different devices used by the application.
 *  @{
 */

/**
 * Definitions and Declarations
 */


// VARIABLES
/**
 * @brief Flag to indicate if the ADS1299 is acquiring data or not
 */
extern bool its_running;
/**
 * @brief Flag to indicate if channel settings commands are being received
 */
extern bool get_channel_settings;
/**
 * @brief Counter used to retrieve channel settings from serial port
 */
extern uint8_t channel_settings_counter;
/**
 * @brief Keep track of what channel we are loading settings for
 */
extern uint8_t channel_selected;
/**
 * @brief Mode of use: 8 channels
 */
extern uint8_t cur_outputType;

/**
 * @brief Flag to indicate if lead off settings commands are being received
 */
extern bool get_leadoff_settings;
/**
 * @brief Counter used to retrieve lead off settings from serial port
 */
extern uint8_t leadoff_settings_counter;

// FUNCTIONS
/**
 * @brief Start the system from scratch, send initial message and get the board ready to get commands
 */
void InitCommunication();

/**
* @description: This is a function that can be called multiple times, this is
*                 what we refer to as a `soft reset`. You will hear/see this
*                 many times.
* @author: AJ Keller (@pushtheworldllc)
*/
void BoardPresentation(void);

/**
 * @brief Function for receiving new commands
 */
void EventSerial(uint8_t data);

/**
 * @brief Function for receiving new commands
 * @param command Character sent from OPENBCI GUI and received by HC05 bluetooth module
 */
void HandleOpenBCICommand(uint8_t command);

/**
 * @brief Function for printing commands menu
 */
void PrintMenu(void);


void init_millis(void);

/**
 * @brief Stop data acquisition
 * @return HIGH if the device is running, LOW if not
 */
bool StopRunning();

/**
 * @brief Start data acquisition
 * @return HIGH if the device is running, LOW if not
 */
bool StartRunning();

/**
 * @brief Send via BLUETOOTH the data acquired by the ADS
 */
 void SendChannelData();


 void SendChannelDataPruebaBT(void);

/**
 * @brief Print the current state of all ADS1299 registers
 */
void PrintRegisters();

/**
* @brief To get and send by UART the name of a specific ADS1299 register
* @param regNumber Number of register in decimal (0-23)
*/
void PrintRegisterName(uint8_t reg);

/**
 * @brief Activate or De-activate channel maintaining running state
 * @param channel Channel number
 * @param start Channel desired state: TRUE (1) to activate channel, FALSE (0) to de-activate channel
 */
void ChangeChannelState(uint8_t channel, uint8_t start);

/**
 * @brief Set test inputs to all channels
 * @param inputCode Input types
 * @param amplitudCode Amplitude of the test signal: ADSTESTSIG_NOCHANGE, ADSTESTSIG_AMP_1X, ADSTESTSIG_AMP_2X
 * @param freqCode Frequence of the test signal: ADSTESTSIG_NOCHANGE, ADSTESTSIG_PULSE_SLOW, ADSTESTSIG_PULSE_FAST
 */
 void AllChannelsToTestCondition(uint8_t testInputCode, uint8_t amplitudeCode, uint8_t freqCode);

/**
 * @brief Set all channels to the default settings
 */
void SetChannelsDefaultSettings();

/**
 * @brief Stop acquisition, set the channels and re-start acquisition
 * @param channel Channel number
 */
void WriteChannelSettings(uint8_t *channel);

/**
 * @brief Set all channels to the default settings
 */
void SetChannelsToDefaultSettings();

/**
 * @brief Print the current Default Channel Settings
 */
void PrintDefaultChannelSettings();

/**
 * @brief Update sampling frequency
 * @param freq Sampling frequency
 */
void UpdateChannelsFrequency(uint8_t freq);

/**
 * @brief Set test inputs to all channels
 * @param inputCode Input types
 * @param amplitudCode Amplitude of the test signal: ADSTESTSIG_NOCHANGE, ADSTESTSIG_AMP_1X, ADSTESTSIG_AMP_2X
 * @param freqCode Frequence of the test signal: ADSTESTSIG_NOCHANGE, ADSTESTSIG_PULSE_SLOW, ADSTESTSIG_PULSE_FAST
 */
 void ActivateAllChannelsTestCondition(uint8_t testInputCode, uint8_t amplitudeCode, uint8_t freqCode);

 /**
  * @brief Send characters to shake hands with the controlling program
  */
 void SendEndOfTransmission();

 /**
  * @brief Retrieve parameters and execute channel settings
  * @param parameter Setting parameter received as command: CHAN_NUM, POWER_DOWN, GAIN_SET, INPUT_TYPE_SET, BIAS_SET, SRB2_SET, SRB1_SET
  */
 void LoadChannelSettings(uint8_t parameter);

 /**
  * @brief Update sampling frequency
  * @param freq Sampling frequency
  */
 void UpdateFrequency(uint8_t freq);


 /**
  * @brief Change lead off detect settings and re-start device
  * @param channel Channel number
  */
 void ChangeChannelLeadOffDetect(uint8_t channel);

 /**
 * @brief Retrieve parameters and execute channel settings
 * @param parameter Setting parameter received as command: PCHAN, NCHAN
 */
void LoadLeadOffSettings(uint8_t parameter);

/**
* @}
*/
#endif /* OPENBCI_COMMUNICATION_H_ */
