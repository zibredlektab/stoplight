#define AIN A1
#define RED 0
#define GREEN 1
#define KNOB A3

int battlvldivided;
int battlvl;
uint32_t lastblinktime = 0;
bool blinkon = false;
int knoblvl = 0;

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
  
  // put your main code here, to run repeatedly:
  battlvldivided = analogRead(AIN); // 0-255
  battlvl = map(battlvldivided, 0, 1023, 0, 2015); // numbers below have been calibrated

  if (battlvl > 1300) { // 13v - 17v = green
    setLED(RED, 0);
    setLED(GREEN, 255);
  } else if (battlvl > 1195) { // 12v - 13v = yellow
    setLED(RED, 200);
    setLED(GREEN, 200);
  } else if (battlvl > 1170) { // 11.5 - 12v = red
    setLED(RED, 255);
    setLED(GREEN, 0);
  } else { // below 11.5 = flash
    setLED(GREEN, 0);

    if (millis() >= lastblinktime + 1000) {
      if (blinkon) {
        setLED(RED, 0);
        blinkon = false;
      } else {
        setLED(RED, 255);
        blinkon = true;
      }
      lastblinktime = millis();
    }
  }
}
