#ifndef ADS1299_TEST_H_
#define ADS1299_TEST_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Run a basic ADS1299 register read/write test.
 *        The function toggles bits in the CONFIG1 register and
 *        verifies that the value is read back correctly.
 * @return true if register accesses succeed.
 */
bool ADS1299_RunRegisterTest(void);

#endif /* ADS1299_TEST_H_ */
