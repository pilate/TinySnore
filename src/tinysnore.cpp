/*
tinysnore.cpp - Library for putting ATtiny*5s to sleep!
Created by Connor Nishijima, May 12th 2017.
Released under the GPLv3 license.
*/

#include "tinysnore.h"
#include <avr/sleep.h>
#include <avr/wdt.h>

#ifndef cbi
	#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
	#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void snore(uint32_t snore_time){
  int i, s;

  cbi(ADCSRA, ADEN);                   // switch Analog to Digitalconverter OFF

  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();
  sleep_bod_disable();

  for (i=8192, s=9; i>=16; i/=2, s--) {
    while (snore_time > i) {
      ts_set_sleep(s);
      ts_system_sleep();
      snore_time -= i;
    }
  }

  if (snore_time) {
    delay(snore_time);
  }

  sbi(ADCSRA, ADEN);                   // switch Analog to Digitalconverter ON
}

void ts_system_sleep(){
  sleep_mode();                        // System sleeps here
  sleep_disable();                     // System continues execution here when watchdog timed out
}

void ts_set_sleep(int ii){
  byte bb;
  int ww;
  if (ii > 9 ) ii = 9;
  bb = ii & 7;
  if (ii > 7) bb |= (1 << 5);
  bb |= (1 << WDCE);
  ww = bb;

  MCUSR &= ~(1 << WDRF);
  // start timed sequence
  WDTCR |= (1 << WDCE) | (1 << WDE);
  // set new watchdog timeout value
  WDTCR = bb;
  WDTCR |= _BV(WDIE);
}

// Watchdog Interrupt Service / is executed when watchdog timed out
ISR(WDT_vect) {
  // NOTHING HERE BY DEFAULT
}
