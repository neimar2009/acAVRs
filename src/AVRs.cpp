
// AVRs.cpp

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "AVRs.h"

// 5 mS é o melhor valor para capacitor de 22uF no pino AREF.
#define waitVref    10
#define loopsVref   5

double readVCC() {

    uint8_t ADMUX_buf = ADMUX;
    // Read 1.1V reference against AVcc
    // set the reference to Vcc and the measurement to the internal 1.1V reference
    #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
    #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
    #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    #endif

    // int ms = 10; // 5 mS é o melhor valor para capacitor de 22uF no pino AREF.
    delay(waitVref);   // Wait for Vref to settle

    uint32_t resultAD = 0;
    for (int i = 0; i < loopsVref; ++i){
        ADCSRA |= _BV(ADSC);               // Start conversion;
        while (bit_is_set( ADCSRA, ADSC)); // measuring;
        resultAD += (ADCL | (ADCH << 8));  // must read ADCL first - it then
                                           // locks ADCH unlocks both.
        delay(1);
    }
    ADMUX = ADMUX_buf; // Estabelesse o estado original.

    // return 1125.3 / (resultAD / loopsVref); // Calculate Vcc (in V); 1125,3 = 1.1*(1024-1)
    return (resolutionBitsResult() * 1.1) / (resultAD / loopsVref);
}

double externalVRef() {

  // TODO: Needs to be tested for release
  return 0.0;

  analogReference(INTERNAL);
  double rtn = analogRead(0) * 0.00107421875; // -> (1.1 / 1024);
  analogReference(EXTERNAL);
  return rtn;
}

// 2.5V * 32k / (32k + 5k) = ~2.2V at the AREF pin.
double readVRef() {
  /*
  DEFAULT:. . . . the default analog reference of 5 volts (on 5V Arduino boards) or 3.3 volts
                  (on 3.3V Arduino boards)
  INTERNAL: . . . an built-in reference, equal to 1.1 volts on the ATmega168 or ATmega328 and
                  2.56 volts on the ATmega8 (not available on the Arduino Mega)
  INTERNAL1V1:. . a built-in 1.1V reference (Arduino Mega only)  (ATtiny too)
  INTERNAL2V56: . a built-in 2.56V reference (Arduino Mega only) (ATtiny too)
  EXTERNAL: . . . the voltage applied to the AREF pin (0 to 5V only) is used as the reference.
  */
  switch (ADMUX >> 6) {
    #if defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
      case DEFAULT  :  return readVCC(); // [DEFAULT  0] Depende da tensão de alimentação.
      case EXTERNAL : {                  // [EXTERNAL 1] Depende da tensão aplicada externamente.
        return externalVRef();
      } /*
      case INTERNAL     : return 1.1;    // [INTERNAL    2] Tensão fixa de 1,1 Volt. */
      case INTERNAL1V1  : return 1.1;    // [INTERNAL1V1 2] Tensão fixa de 1,1 Volt.
    #elif defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
      case DEFAULT  :  return readVCC(); // [DEFAULT  0] Depende da tensão de alimentação.
      case EXTERNAL : {                  // [EXTERNAL 4] Depende da tensão aplicada externamente.
        return externalVRef();
      } /*
      case INTERNAL     : return 1.1;        // [INTERNAL    8] Tensão fixa de 1,1 Volt. */
      case INTERNAL1V1  : return 1.1;        // [INTERNAL1V1 8] Tensão fixa de 1,1 Volt.
      case INTERNAL2V56 : return 2.56;       // [INTERNAL    9] Tensão fixa de 2,56 Volt.
      case INTERNAL2V56_EXTCAP : return 2.56;// [INTERNAL2V56_EXTCAP 13] Tensão fixa de 2,56 Volt.
    #else
    #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)  ||    \
        defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) ||    \
        defined(__AVR_ATmega644__)  || defined(__AVR_ATmega644A__)  ||    \
        defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)
      case INTERNAL1V1  : return 1.1;    // [INTERNAL1V1  2] Tensão fixa de 1,1 Volt.
      case INTERNAL2V56 : return 2.56;   // [INTERNAL2V56 3] Tensão fixa de 2,56 Volt.
    #else
      case INTERNAL : return 1.1;        // [INTERNAL 3] Tensão fixa de 1,1 Volt.
    #endif
      case EXTERNAL : {                  // [EXTERNAL 0] Depende da tensão aplicada externamente.
        return 0.0;
      }
      case DEFAULT  :  return readVCC(); // [DEFAULT  1] Depende da tensão de alimentação.
    #endif
  }

  // return 0.0;
}

uint8_t resolutionBits() {

  #if defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__) ||  \
      defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__) ||  \
      defined(__AVR_ATmega328__)  || defined(__AVR_ATmega328P__)  ||    \
      defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)  ||    \
      defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) ||    \
      defined(__AVR_ATmega644__)  || defined(__AVR_ATmega644A__)  ||    \
      defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)
    return 10;
  #else
    return 12;
  #endif
}

uint16_t resolutionBitsResult() {

  return pow( 2, resolutionBits());
}



