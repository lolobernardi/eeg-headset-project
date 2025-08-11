/*
 * ADS1299_Driver.h
 * Driver for ADS1299 EEG front-end
 */

#ifndef ADS1299_DRIVER_H_
#define ADS1299_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>
#include "ADS1299_Parameters.h"

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
 * @brief Write default channel configuration to device.
 */
void ADS1299_WriteDefaultChannelSettings(void);

/* Compatibility wrapper for existing code */
static inline void Drv_ADS1299_initialize(void) { ADS1299_Init(); }

#endif /* ADS1299_DRIVER_H_ */
