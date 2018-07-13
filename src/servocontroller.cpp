#include "Servocontroller.h"

#ifdef DEBUG
  #include <cmath>
#else
  #include <math.h>
  #include <util/delay.h>
#endif

//setzt adresse des Servo Controller -> default = 0x40 und erzeugt TwoWire Objekt zur Übertragung
Servocontroller::Servocontroller(uint8_t addr)
: i2caddr{addr}, twi{Twi {100000L}} {
  reset();
  setPWMFreq(50);
}

//resettet Controller
void Servocontroller::reset() {
  write8(PCA9685_MODE1, 0x80);
  //allow time for oscillator to stabilize
  #ifndef DEBUG
  _delay_us(500);
  #endif
}

//Setzt die Frequenz der PWM Signale
void Servocontroller::setPWMFreq(float freq) {
  //Calculate prescaler to get the right frequenzy
  //prescale = round(clock/4096*freq) -1
  //uint8_t prescale = round(25000000/(4096*freq)) -1;

  uint8_t prescale = round(25000000/(4096*(0.9*freq)) - 0.5);

  //Put Servo controller into the sleep mode, so the frequenzy can be set. Afterwards awake the Servocontroller.
  uint8_t oldmode = read8(PCA9685_MODE1);
  //sleeo mode
  write8(PCA9685_MODE1, (oldmode&0x7F) | 0x10);
  //set prescale
  write8(PCA9685_PRESCALE, prescale);
  //awake from sleep mode
  write8(PCA9685_MODE1, oldmode);

  //allow time for oscillator to stabilize
  #ifndef DEBUG
  _delay_us(500);
  #endif

  //turn on auto increment
  write8(PCA9685_MODE1, oldmode | 0xa0);
}

//Erzeugt ein PWM Signal am pin pinNum mit dem übergebenem on und off wert im Zyklus
void Servocontroller::setPWM(uint8_t pinNum, uint16_t on, uint16_t off) {
    uint8_t buffer[] = {LED0_ON_L+4*pinNum, on, on>>8, off, off>>8};
    twi.writeTo(i2caddr, buffer, 6);
}

/*******************************************************************************************/
//Hilfsfunktionen
uint8_t Servocontroller::read8(uint8_t addr) {
    twi.writeTo(i2caddr, &addr, 1);
    uint8_t data;
    twi.readFrom(i2caddr, &data, 1);
    return data;
}

void Servocontroller::write8(uint8_t addr, uint8_t d) {
    uint8_t buffer[] = {addr, d};
    twi.writeTo(i2caddr, buffer, 2);
}
