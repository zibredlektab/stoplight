#define AIN A1
#define RED 3
#define GREEN 4

int battlvldivided;
int battlvl;
uint32_t lastblinktime = 0;
bool blinkon = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(AIN, INPUT); // battery voltage divider
  pinMode(RED, OUTPUT); // red led
  pinMode(GREEN, OUTPUT); // green led
}

void loop() {

  
  // put your main code here, to run repeatedly:
  battlvldivided = analogRead(AIN); // 0-255
  battlvl = map(battlvldivided, 0, 1023, 0, 2015); // numbers below have been calibrated

  if (battlvl > 1300) { // 13v - 17v = green
    analogWrite(RED, 255);
    analogWrite(GREEN, 0);
  } else if (battlvl > 1195) { // 12v - 13v = yellow
    analogWrite(RED, 100);
    analogWrite(GREEN, 100);
  } else if (battlvl > 1170) { // 11.5 - 12v = red
    analogWrite(RED, 0);
    analogWrite(GREEN, 255);
  } else { // below 11.5 = flash
    analogWrite(GREEN, 255);

    if (millis() >= lastblinktime + 1000) {
      if (blinkon) {
        analogWrite(RED, 255);
        blinkon = false;
      } else {
        analogWrite(RED, 0);
        blinkon = true;
      }
      lastblinktime = millis();
    }
  }
}
