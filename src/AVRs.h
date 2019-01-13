#ifndef AVRs_h
#define AVRs_h

#include <Arduino.h>

// Lê a tensão de alimentação do AVR.
// Retorna: A média de 5 leituras.
double readVCC();

// Obtém o valor da tensão de referência VRef.
// Retorna: A tensão de referência.
double readVRef();

// Número de bits 
uint8_t resolutionBits();

// Valor resultante do número de bits.
uint16_t resolutionBitsResult();

#endif // AVRs_h
