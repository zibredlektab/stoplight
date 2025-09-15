#include <EEPROM.h>

#define BATT A1
#define RED 1
#define GREEN 0
#define BUTTON 3

#define HYSTERESIS 0.5
#define DEBOUNCE 100

#define GREENTOYELLOW 13.0
#define YELLOWTORED 12.5
#define REDTOFLASH 11.9


int count = 0; // count of ADC samples taken
uint32_t battlvltemp = 0; // for averaging
long battlvlmean = 0; // mean of ADC input
float battlvl = 0.0; // mapped to voltage
float prevbattlvl = 0.0; // voltage last loop

uint32_t lastblinktime = 0; // low voltage alert blink
bool blinking = false;
bool blinkon = false;

int dimlvl; // 0-2, 2 being brightest
uint32_t lastbuttontime = 0; // button debounce
bool buttonreleased = false;


void setup() {
  // put your setup code here, to run once:
  pinMode(BATT, INPUT); // battery voltage divider
  pinMode(BUTTON, INPUT_PULLUP); // dimmer
  pinMode(RED, OUTPUT); // red led
  pinMode(GREEN, OUTPUT); // green led

  EEPROM.get(1, dimlvl);

/*
  delay(1000);

  setLED(RED, 255);
  setLED(GREEN, 0);
  delay(500);
  setLED(RED, 0);
  setLED(GREEN, 0);
  delay(100);
  setLED(RED, 210);
  setLED(GREEN, 80);
  delay(500);
  setLED(RED, 0);
  setLED(GREEN, 0);
  delay(100);
  setLED(RED, 0);
  setLED(GREEN, 255);
  delay(500);
  setLED(RED, 0);
  setLED(GREEN, 0);
  delay(1000);
  */
  
}

void setLED(int pin, int lvl) {

  int dim = 4;
  if (dimlvl == 0) dim = 4;
  if (dimlvl == 1) dim = 2;
  if (dimlvl == 2) dim = 1;

  analogWrite(pin, lvl/dim);
}

void loop() {

  /*
   if batt level is higher than it was the last loop, it must be at least X higher in order to switch.
   lower batt level than last loop can switch instantly
  */

  count = 0;
  while (count++ < 100) {
    // take lots of readings to smooth everything out
    battlvltemp += analogRead(BATT);
    delay(1);
  }
  battlvltemp /= 100; // take the mean
  battlvlmean = battlvltemp;
  battlvl = battlvlmean * (5.0 / 1023.0); // ADC reading to voltage
  battlvl /= 0.248; // undo voltage divider (more precise resistors will help)
  battlvl -= 0.2; // correct for offset
  //battlvl = map(battlvlmean, 0, 1023, 0, 2015); // numbers below have been calibrated


  if ((battlvl > prevbattlvl) && (battlvl - prevbattlvl < HYSTERESIS)) {
    // voltage has increased, but by too small a value to matter
    // we do nothing
    battlvl = prevbattlvl;
  } else {
    // voltage has either gone up, not changed, or gone down
    prevbattlvl = battlvl;
  }

  prevbattlvl = battlvl;
  if (battlvl > GREENTOYELLOW) { // 13v - 17v = green
    setLED(RED, 0);
    setLED(GREEN, 255);
    blinking = false;
  } else if (battlvl > YELLOWTORED) { // 12.4v - 13v = yellow
    setLED(RED, 210);
    setLED(GREEN, 50);
    blinking = false;
  } else if (battlvl > REDTOFLASH) { // 12.1 - 12.4v = red
    setLED(RED, 255);
    setLED(GREEN, 0);
    blinking = false;
  } else { // below 12.1 = flash
    setLED(GREEN, 0);
    blinking = true;
  }

  if (blinking) {
    if (millis() >= lastblinktime + 1000) {
      if (blinkon) {
        setLED(RED, 60);
        blinkon = false;
      } else {
        setLED(RED, 255);
        blinkon = true;
      }
      lastblinktime = millis();
    }
  }

  if (digitalRead(BUTTON)) { // button is up
    if (buttonreleased && millis() >= lastbuttontime + DEBOUNCE) { // button was not previously up (just released)
      buttonreleased = false;
      dimlvl++;
      if (dimlvl > 2) dimlvl = 0;
      EEPROM.put(1, dimlvl);
      lastbuttontime = millis();
    }
  } else {
    buttonreleased = true;
  }
}
