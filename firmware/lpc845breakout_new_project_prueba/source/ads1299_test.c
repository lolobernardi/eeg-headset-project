#include "ads1299_test.h"
#include "ADS1299_Driver.h"
#include "fsl_debug_console.h"


// Convierte un nibble (4 bits) a caracter hex

static inline char nyb_to_hex(uint8_t v) {
    v &= 0xF;
    return (v < 10) ? ('0' + v) : ('A' + v - 10);
}

// Imprime "texto" seguido de valor en hex y \r\n

void USART_PrintHexByte(USART_Type *base, const char *texto, uint8_t valor)
{
    // Enviar primero el texto (sin el \0 final)
    size_t len = 0;
    while (texto[len] != '\0') len++;
    USART_WriteBlocking(base, (const uint8_t *)texto, len);

    // Armar los dos dígitos hexadecimales
    char hex[4];
    hex[0] = nyb_to_hex(valor >> 4);
    hex[1] = nyb_to_hex(valor);
    hex[2] = '\r';
    hex[3] = '\n';

    // Enviar el valor + terminador
    USART_WriteBlocking(base, (uint8_t *)hex, sizeof(hex));
}


bool ADS1299_RunRegisterTest(void)
{
    uint8_t prueba = 0x22;
    USART_PrintHexByte(USART0,"Entra test: 0x", prueba);

    uint8_t original = ADS1299_ReadRegister(CONFIG1);

    //uint8_t prueba2 = 0x54;

    USART_PrintHexByte(USART0,"Prueba 1: 0x", original);
    //USART_PrintHexByte(USART0,"Prueba 2: 0x", prueba2);

    USART_PrintHexByte(USART0,"Sale test: 0x", prueba);

    /*
    USART_PrintHexByte(USART0,"CONFIG1 initial: 0x", original);

    uint8_t testVal = original ^ 0x05U;
    ADS1299_WriteRegister(CONFIG1, testVal);
    uint8_t readBack = ADS1299_ReadRegister(CONFIG1);

    USART_PrintHexByte(USART0,"CONFIG1 written: 0x", testVal);
    USART_PrintHexByte(USART0,"Read back: 0x", readBack);

    // Restore original value
    ADS1299_WriteRegister(CONFIG1, original);

    bool pass = (readBack == testVal);
    return pass;*/
}


bool ADS1299_RunRegisterTest2(void)
{
    PRINTF("CONFIG1 initial:LOLO");
}
