/*
 * ADS1299_Driver.h
 *
 *  Created on: 7 ago. 2025
 *      Author: Usuario
 */

#ifndef ADS1299_DRIVER_H_
#define ADS1299_DRIVER_H_

#include "fsl_common.h"
#include "ADS1299_Parameters.h"
/**
 * @brief Configure pins connected to the ADS
 */
void ADS1299_InitPins(void);

/**
 * @brief Configure the ADS itself
 */
void Drv_ADS1299_initialize();

/**
 * @brief Hard Reset ADS and power up sequence
 */
//void Drv_ADS1299_initialize_ads();

/**
 * @brief Stop data acquisition
 */
//void Drv_ADS1299_stopStreaming();

/**
 * @brief Start data acquisition
 */
//void Drv_ADS1299_startStreaming();

/**
 * @brief Start continuous data acquisition
 */
void Drv_ADS1299_startADS();

/**
 * @brief Stop continuous data acquisition
 */
void Drv_ADS1299_stopADS();

/**
 * @brief Get out of low power mode
 * @param targetSS SPI target to talk to: BOARD_ADS, SLAVE_ADS or BOTH_ADS
 */
void Drv_ADS1299_WAKEUP(uint8_t targetSS);

/**
 * @brief Go into low power mode
 * @param targetSS SPI target to talk to: BOARD_ADS, SLAVE_ADS or BOTH_ADS
 */
void Drv_ADS1299_STANDBY(uint8_t targetSS);

/**
 * @brief Set all register values to default
 * @param targetSS SPI target to talk to: BOARD_ADS, SLAVE_ADS or BOTH_ADS
 */
void Drv_ADS1299_RESET(uint8_t targetSS);

/**
 * @brief Start data acquisition
 * @param targetSS SPI target to talk to: BOARD_ADS, SLAVE_ADS or BOTH_ADS
 */
void Drv_ADS1299_START(uint8_t targetSS);

/**
 * @brief Stop data acquisition
 * @param targetSS SPI target to talk to: BOARD_ADS, SLAVE_ADS or BOTH_ADS
 */
void Drv_ADS1299_STOP(uint8_t targetSS);

/**
 * @brief Go into read data continuous mode
 * @param targetSS SPI target to talk to: BOARD_ADS, SLAVE_ADS or BOTH_ADS
 */
void Drv_ADS1299_RDATAC(uint8_t targetSS);

/**
 * @brief Get out of read data continuous mode
 * @param targetSS SPI target to talk to: BOARD_ADS, SLAVE_ADS or BOTH_ADS
 */
//void Drv_ADS1299_SDATAC(uint8_t targetSS);

/**
 * @brief Read data one-shot
 * @param targetSS SPI target to talk to: BOARD_ADS, SLAVE_ADS or BOTH_ADS
 */
void Drv_ADS1299_RDATA(uint8_t targetSS);

/**
 * @brief Read one ADS register
 * @param _address Register address to read
 * @param targetSS SPI target to talk to: BOARD_ADS, SLAVE_ADS or BOTH_ADS
 * @return Requested register value
 */
uint8_t Drv_ADS1299_RREG(uint8_t _address, uint8_t targetSS);

/**
 * @brief  Read multiple ADS registers
 * @param _address Register address to read
 * @param _numRegistersMinusOne Number of registers to read minus one
 * @param targetSS SPI target to talk to: BOARD_ADS, SLAVE_ADS or BOTH_ADS
 */
//void Drv_ADS1299_RREGS(uint8_t _address, uint8_t _numRegistersMinusOne, uint8_t targetSS);

/**
 * @brief Write one ADS register
 * @param _address Register address to write
 * @param _value Byte to write in register
 * @param targetSS SPI target to talk to: BOARD_ADS, SLAVE_ADS or BOTH_ADS
 */
void Drv_ADS1299_WREG(uint8_t _address,uint8_t _value, uint8_t targetSS);

/**
 * @brief Write multiple ADS registers
 * @param _address Register address to write
 * @param _numRegistersMinusOne Number of registers to write minus one
 * @param targetSS SPI target to talk to: BOARD_ADS, SLAVE_ADS or BOTH_ADS
 */
//void Drv_ADS1299_WREGS(uint8_t _address, uint8_t _numRegistersMinusOne, uint8_t targetSS);

/**
 * @brief SPI chip select method
 * @param SS SPI target to talk to: BOARD_ADS, SLAVE_ADS or BOTH_ADS
 */
void Drv_ADS1299_csLow(uint8_t SS);

/**
 * @brief SPI chip de-select
 * @param SS SPI target to talk to: BOARD_ADS, SLAVE_ADS or BOTH_ADS
 */
void Drv_ADS1299_csHigh(uint8_t SS);

/**
 * @brief SPI communication method
 * @param _data Byte to transfer
 * @return Byte received from SPI target
 */
uint8_t Drv_ADS1299_xfer(uint8_t _data);

/**
 * @brief Write settings of only one specific channel
 * @param N Number of the channel to set (1-16)
 */
void Drv_ADS1299_writeOneChannelSettings(uint8_t N);

/**
 * @brief Write settings of all channels
 */
void Drv_ADS1299_writeAllChannelSettings();

/**
 * @brief Set all channels with the default values
 */
void Drv_ADS1299_setChannelsToDefault();

/**
 * @brief Activate specific channel N
 * @param N Number of the channel to activate (1-16)
 */
void Drv_ADS1299_activateChannel(uint8_t N);

/**
 * @brief De-activate specific channel N
 * @param N Number of the channel to de-activate (1-16)
 */
void Drv_ADS1299_deactivateChannel(uint8_t N);

/**
 * @brief Constrains a number to be within a range.
 * @param num Number to constrain
 * @param min Lower limit
 * @param max Upper limit
 * @return New number between the limits
 */
uint8_t Drv_ADS1299_constrain(uint8_t num, uint8_t min, uint8_t max);

/**
 * @brief Query to see if data is available from the ADS1299
 * @return TRUE is data is available, FALSE otherwise
 */
bool Drv_ADS1299_isDataAvailable();

/**
 * @brief Return the ID of the AFE device
 * @param targetSS SPI target to talk to: BOARD_ADS, SLAVE_ADS or BOTH_ADS
 * @return ID of the AFE device
 */
uint8_t Drv_ADS1299_getDeviceID(uint8_t targetSS);

/**
 * @brief Function called to receive new data from the ADS1299
 */
void Drv_ADS1299_updateChannelData();

/**
 * @brief Function called to receive new data from the master board
 */
void Drv_ADS1299_updateBoardData();

 /**
 * @brief Function called to receive new data from the slave board
 */
void Drv_ADS1299_updateSlaveData();

/**
 * @brief Configure the test signals that can be inernally generated by the ADS1299
 * @param amplitudCode Amplitude of the test signal: ADSTESTSIG_NOCHANGE, ADSTESTSIG_AMP_1X, ADSTESTSIG_AMP_2X
 * @param freqCode Frequence of the test signal: ADSTESTSIG_NOCHANGE, ADSTESTSIG_PULSE_SLOW, ADSTESTSIG_PULSE_FAST
 */
void Drv_ADS1299_configureInternalTestSignal(uint8_t amplitudeCode, uint8_t freqCode);

/**
 * @brief Change the source of signal connected to the inputs of the ADS1299
 * @param inputCode Input types
 */
void Drv_ADS1299_changeInputType(uint8_t inputCode);

/**
 * @brief Return a array with ADS1299 registers data
 * @param *regArray Pointer to an array with 24 elements to store ADS registers values
 */
void Drv_ADS1299_getRegistersData(uint8_t *regArray);

/**
 * @brief To know if the Slave Board is present
 * @return HIGH if Slave Board is present, LOW if not
 */
bool Drv_ADS1299_getSlavePresent();

/**
 * @brief Return an array with the 6 values of default channel settings
 * @param *defChanSettings Pointer to an array with 6 elements to store Default Channel Settings
 */
void Drv_ADS1299_getDefaultChannelSettings(uint8_t *defChanSettings);

/**
 * @brief Change the lead off detect settings for specified channel
 * @param N Number of the channel to de-activate (1-16)
 */
void Drv_ADS1299_changeOneChannelLeadOffDetect(uint8_t N);

/**
 * @brief Change the lead off detect settings for all channels
 */
void Drv_ADS1299_changeAllChannelLeadOffDetect();

/**
 * @brief Check if Slave Board is present
 * @return HIGH if Slave board is present, LOW if not
 */
bool Drv_ADS1299_smellSlave();

/**
 * @brief Stop Slave ADS and remove link
 */
void Drv_ADS1299_removeSlave();

/**
 * @brief Configure CLK output and detect Slave board
 */
void Drv_ADS1299_attachSlave();

/**
 * @brief Reset all the ADS1299 settings. Stops all data acquisition
 * @param targetSS SPI target to talk to: BOARD_ADS, SLAVE_ADS or BOTH_ADS
 */
void Drv_ADS1299_resetADS(uint8_t targetSS);

/**
 * @brief Function to get Channel Data
 * @param &sampleCnt Pointer to a uint8_t variable to store Sample Count
 * @param *data Pointer to an array with 48 elements (24 of Mastar board data and 24 of Slave board data)
 */
void Drv_ADS1299_getChannelData(uint8_t *sampleCnt, uint8_t *data);




#endif /* ADS1299_DRIVER_H_ */
