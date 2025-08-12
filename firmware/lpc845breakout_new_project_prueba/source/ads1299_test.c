#include "ads1299_test.h"
#include "ADS1299_Driver.h"
#include "fsl_debug_console.h"

bool ADS1299_RunRegisterTest(void)
{
    uint8_t original = ADS1299_ReadRegister(CONFIG1);
    PRINTF("CONFIG1 initial: 0x%02X\r\n", original);

    uint8_t testVal = original ^ 0x05U; /* Flip a couple of bits */
    ADS1299_WriteRegister(CONFIG1, testVal);
    uint8_t readBack = ADS1299_ReadRegister(CONFIG1);
    PRINTF("CONFIG1 written: 0x%02X, read back: 0x%02X\r\n", testVal, readBack);

    /* Restore original value */
    ADS1299_WriteRegister(CONFIG1, original);

    bool pass = (readBack == testVal);
    PRINTF("Register R/W %s\r\n", pass ? "OK" : "FAILED");
    return pass;
}
