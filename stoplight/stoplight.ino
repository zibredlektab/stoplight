#define AIN 2
#define RED 3
#define GREEN 4

uint8_t battlvldivided;
uint16_t battlvl;
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
  battlvl = map(battlvldivided, 0, 255, 0, 2015); // this...should map out??

  if (battlvl > 1300) { // 13v - 17v = green
    digitalWrite(RED, 0);
    digitalWrite(GREEN, 0);
  } else if (battlvl > 1200) { // 12v - 13v = yellow
    analogWrite(RED, 128);
    analogWrite(GREEN, 128);
  } else if (battlvl > 1150) { // 11.5 - 12v = red
    digitalWrite(RED, 1);
    digitalWrite(GREEN, 0);
  } else { // below 11.5 = flash
    digitalWrite(GREEN, 0);

    if (millis() >= lastblinktime + 1000) {
      digitalWrite(RED, !blinkon);
      blinkon = !blinkon;
      lastblinktime = millis();
    }
  }
}
