#ifndef ADS1299_TEST_H_
#define ADS1299_TEST_H_

#include <stdbool.h>
#include <stdint.h>
#include "board.h"
#include "app.h"
#include "fsl_usart.h"

/**
 * @brief Run a basic ADS1299 register read/write test.
 *        The function toggles bits in the CONFIG1 register and
 *        verifies that the value is read back correctly.
 * @return true if register accesses succeed.
 */
bool ADS1299_RunRegisterTest(void);
bool ADS1299_RunRegisterTest2(void);
static inline char nyb_to_hex(uint8_t v);
void USART_PrintHexByte(USART_Type *base, const char *texto, uint8_t valor);

#endif /* ADS1299_TEST_H_ */
