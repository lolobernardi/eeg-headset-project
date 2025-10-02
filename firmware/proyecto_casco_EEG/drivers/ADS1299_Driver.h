/*
 * ADS1299_Driver.h
 * Driver for ADS1299 EEG front-end
 */

#ifndef ADS1299_DRIVER_H_
#define ADS1299_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>
#include "ADS1299_Parameters.h"

#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)

void ADS1299_writeNewChannelSettings(uint8_t* newChannelSettings);

void ADS1299_configureInternalTestSignal(uint8_t amplitudeCode,uint8_t freqCode);

void ADS1299_changeInputType(uint8_t testInputCode);

void ADS1299_WriteOneChannelSettings(uint8_t channel);

void ADS1299_WriteChannelSettings(void);

void ADS1299_StartStreaming(void);

void ADS1299_UpdateChannelData(void);

void ADS1299_SendChannelDataUART(void);

void ADS1299_SendChannelDataOpenBCI(void);

void ADS1299_copyLatest(int32_t out[8]);

bool ADS1299_TestChannelRegisters(void);
/**
 * @brief Initialize GPIOs, SPI and the ADS1299 device.
 */
void ADS1299_Init(void);

/**
 * @brief Start continuous conversions.
 */
void ADS1299_Start(void);

/**
 * @brief Stop continuous conversions.
 */
void ADS1299_Stop(void);

/**
 * @brief Reset the ADS1299 device.
 */
void ADS1299_Reset(void);

/**
 * @brief Read a register from the ADS1299.
 * @param reg Register address.
 * @return Register value.
 */
uint8_t ADS1299_ReadRegister(uint8_t reg);

/**
 * @brief Write a register in the ADS1299.
 * @param reg Register address.
 * @param value Byte to write.
 */
void ADS1299_WriteRegister(uint8_t reg, uint8_t value);

/**
 * @brief Configure sampling rate in CONFIG1 register.
 * @param rate One of SAMPLE_RATE_* values.
 */
void ADS1299_SetSampleRate(uint8_t rate);

/**
 * @brief Set the PGA gain for a channel.
 * @param channel Channel number 1-8.
 * @param gain Use ADS_GAINxx values.
 */
void ADS1299_SetChannelGain(uint8_t channel, uint8_t gain);

/**
 * @brief Check if new data are available.
 * @return true when DRDY is low.
 */
bool ADS1299_IsDataReady(void);

/**
 * @brief Read one sample from all channels.
 * @param channelData Pointer to array of 8 int32_t where the channel data will be stored.
 * @param status Optional pointer to store the 24-bit status word.
 */
void ADS1299_ReadData(int32_t *channelData, uint32_t *status);

/**
 * @brief Read device ID.
 * @return Value of ID register.
 */
uint8_t ADS1299_GetDeviceID(void);

/**
 * @brief Power down one channel.
 * @param channel Channel number 1-8.
 */
void ADS1299_DeactivateChannel(uint8_t channel);

/**
 * @brief Activate a channel using current settings.
 * @param channel Channel number 1-8.
 */
void ADS1299_ActivateChannel(uint8_t channel);

/**
 * @brief Write default channel configuration to device.
 */
void ADS1299_WriteDefaultChannelSettings(void);


#endif /* ADS1299_DRIVER_H_ */
