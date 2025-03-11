#define BATT A2
#define RED 1
#define GREEN 0
#define KNOB A3

#define HYSTERESIS 10


int count = 0; // count of ADC samples taken
uint32_t battlvltemp = 0; // for averaging
long battlvlmean = 0; // mean of ADC input
float battlvl = 0.0; // mapped to voltage
float prevbattlvl = 0.0; // voltage last loop

uint32_t lastblinktime = 0; // low voltage alert blink
bool blinking = false;
bool blinkon = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(BATT, INPUT); // battery voltage divider
  pinMode(KNOB, INPUT); // dimmer
  pinMode(RED, OUTPUT); // red led
  pinMode(GREEN, OUTPUT); // green led
}

void setLED(int pin, int lvl) {
  long knoblvl = 0;
  int knobcount = 0;
  while (knobcount < 10) {
    knoblvl += analogRead(KNOB);
    knobcount++;
  }
  knobcount = 0;
  knoblvl /= 10;
  int dim = map(knoblvl, 0, 1023, 10, 1);
  analogWrite(pin, 255 - (lvl/dim));
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
  if (battlvl > 13.0) { // 13v - 17v = green
    setLED(RED, 0);
    setLED(GREEN, 255);
    blinking = false;
  } else if (battlvl > 12.4) { // 12.4v - 13v = yellow
    setLED(RED, 100);
    setLED(GREEN, 100);
    blinking = false;
  } else if (battlvl > 12.1) { // 12.1 - 12.4v = red
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
        setLED(RED, 100);
        blinkon = false;
      } else {
        setLED(RED, 255);
        blinkon = true;
      }
      lastblinktime = millis();
    }
  }
}
