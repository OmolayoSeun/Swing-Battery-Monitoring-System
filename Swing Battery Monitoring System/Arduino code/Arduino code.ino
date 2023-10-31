#include <LiquidCrystal_I2C.h>
#define toBat A2
#define toLoad A0

LiquidCrystal_I2C lcd(0x27, 16, 2);
float sensorValue = 0.0;
float current = 0;
byte cSymbol[] = { B00001, B00010, B00100, B01111, B11110, B00100, B01000, B10000 };
byte cNull[] = { B00000, B00000, B00000, B00000, B00000, B00000, B00000, B00000 };
byte cConnect[] = { B00100, B01110, B10101, B00100, B00100, B00100, B00100, B00100 };
byte cSend[] = { B00100, B01110, B10101, B00100, B00100, B00100, B00101, B00101 };
byte cFail[] = { B00100, B01110, B10101, B00110, B00100, B01100, B10100, B00100 };
long int prev = 0;

float getToLoad() {
  sensorValue = 0.0;

  for (byte i = 0; i < 50; i++) {
    sensorValue += analogRead(toLoad);
    delay(2);
  }
  sensorValue = (sensorValue / 50.0);
  sensorValue = ((sensorValue * 5.0 / 1024.0) - 2.5) / 0.066;
  
  
  return (sensorValue > 0.0) ? sensorValue : 0.0;
}

float getToBat() {

  sensorValue = 0.0;
  for (byte i = 0; i < 50; i++) {
    sensorValue += analogRead(toBat);
    delay(2);
  }
  sensorValue = (sensorValue / 50.0);
  sensorValue = ((sensorValue * 5.0 / 1024.0) - 2.49) / 0.066;
  
  return (sensorValue > 0.0) ? sensorValue : 0.0;
}

void display(float charg, float load, float volt) {
  //lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CHAR");
  lcd.setCursor(0, 1);
  lcd.print(charg);

  if (charg != 0.0) {
    lcd.setCursor(15, 0);
    lcd.write(byte(0));
  } else {
    lcd.setCursor(15, 0);
    lcd.write(byte(1));
  }


  lcd.setCursor(5, 0);
  lcd.print("LOAD");
  lcd.setCursor(5, 1);
  lcd.print(load);

  lcd.setCursor(10, 0);
  lcd.print("VOLT");
  lcd.setCursor(10, 1);
  lcd.print(volt);

  if ((millis() - prev) > 16000) {
    String msg = "";
    msg.concat(charg);
    msg += ":";
    msg.concat(load);
    msg += ":";
    msg.concat(volt);
    msg += ":";
    Serial.println(msg);

    prev = millis();
  }

  //delay(500);
}

float readVolt() {

  sensorValue = 0.0;
  for (byte i = 0; i < 50; i++) {
    sensorValue += analogRead(A1);
    delay(2);
  }
  sensorValue = sensorValue / 50.0;
  sensorValue = (sensorValue * 5.0) / 1024.0;
  sensorValue = (sensorValue / (8060.0 / 25970.0)) - 0.32;
  return (sensorValue > 0.0) ? sensorValue : 0.0;
}

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, cSymbol);
  lcd.createChar(1, cNull);
  lcd.createChar(2, cConnect);
  lcd.createChar(3, cSend);
  lcd.createChar(4, cFail);
  Serial.begin(9600);


  lcd.setCursor(15, 1);
  lcd.write(byte(4));
}

void loop() {
  display(getToBat(), getToLoad(), readVolt());

  if (Serial.available()) {

    switch (Serial.read()) {
      case 'Y':
        lcd.setCursor(15, 1);
        lcd.write(byte(3));
        break;

      case 'N':
        lcd.setCursor(15, 1);
        lcd.write(byte(4));
        break;

      case 'C':
        lcd.setCursor(15, 1);
        lcd.write(byte(2));
        break;
    }
  }

  delay(200);
}