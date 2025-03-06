#define AIN A2
#define RED 1
#define GREEN 0
#define KNOB A3

#define HYSTERESIS 10


int count = 0; // count of ADC samples taken
uint32_t battlvltemp = 0; // for averaging
int battlvldivided = 0; // mean of ADC input
int battlvl = 0; // mapped to voltage
int prevbattlvl = 0; // voltage last loop

uint32_t lastblinktime = 0; // low voltage alert blink
bool blinking = false;
bool blinkon = false;

int knoblvl = 0; // for dimming

void setup() {
  // put your setup code here, to run once:
  pinMode(AIN, INPUT); // battery voltage divider
  pinMode(KNOB, INPUT); // dimmer
  pinMode(RED, OUTPUT); // red led
  pinMode(GREEN, OUTPUT); // green led
}

void setLED(int pin, int lvl) {
  knoblvl = map(analogRead(KNOB), 0, 1023, 10, 1);
  analogWrite(pin, 255 - (lvl/knoblvl));
}

void loop() {

  /*
   if batt level is higher than it was the last loop, it must be at least X higher in order to switch.
   lower batt level than last loop can switch instantly
  */

  count = 0;
  while (count++ < 100) {
    battlvltemp += analogRead(AIN);
    delay(1);
  }
  battlvltemp /= 100;
  battlvldivided = battlvltemp;
  battlvl = map(battlvldivided, 0, 1023, 0, 2015); // numbers below have been calibrated

  if ((battlvl > prevbattlvl) && (battlvl - prevbattlvl < HYSTERESIS)) {
    // voltage has increased, but by too small a value to matter
    // we do nothing
  } else {
    // voltage has either gone up, not changed, or gone down
    prevbattlvl = battlvl;
    if (battlvl > 1300) { // 13v - 17v = green
      setLED(RED, 0);
      setLED(GREEN, 255);
      blinking = false;
    } else if (battlvl > 1200) { // 12v - 13v = yellow
      setLED(RED, 200);
      setLED(GREEN, 200);
      blinking = false;
    } else if (battlvl > 1170) { // 11.5 - 12v = red
      setLED(RED, 255);
      setLED(GREEN, 0);
      blinking = false;
    } else { // below 11.5 = flash
      setLED(GREEN, 0);
      blinking = true;
    }
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
