/*
  LiquidCrystal Library - Hello World

  Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
  library works with all LCD displays that are compatible with the
  Hitachi HD44780 driver. There are many of them out there, and you
  can usually tell them by the 16-pin interface.

  This sketch prints "Hello World!" to the LCD
  and shows the time.

  The circuit:
   LCD RS pin to digital pin 12
   LCD Enable pin to digital pin 11
   LCD D4 pin to digital pin 5
   LCD D5 pin to digital pin 4
   LCD D6 pin to digital pin 3
   LCD D7 pin to digital pin 2
   LCD R/W pin to ground
   LCD VSS pin to ground
   LCD VCC pin to 5V
   LCD VO to digital pin 9
   LCD Backlight+ to digital pin 10

*/

// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2, contrast = 9, brightness = 10;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// create btc char
byte btc_char[8] = {
  B01010,
  B11110,
  B10001,
  B11110,
  B10001,
  B11110,
  B01010,
  B00000
};

byte sat_char[8] = {
  B00100,
  B01110,
  B00000,
  B01110,
  B00000,
  B01110,
  B00100,
  B00000
};

// common variables
const int BAUD_RATE = 19200;
const int sleepTime = 5000;
String height = "";
String btcPrice = "";
String satsPrice = "";
String coinsInCirculation = "";
String halvingBlocks = "";

void setup() {
  // set up pwm pins
  pinMode(contrast, OUTPUT);
  pinMode(brightness, OUTPUT);

  // set up the LCD
  lcd.begin(16, 2);
  lcd.noCursor(); // hide the cursor

  // set LCD brightness and contrast
  analogWrite(brightness, 85); // 1/3 of max 255
  analogWrite(contrast, 50);   // lower = more contrast

  // tell the LCD about our custom chars
  lcd.createChar(0, btc_char);
  lcd.createChar(1, sat_char);

  // Start serial comms
  Serial.begin(BAUD_RATE);
  while (!Serial) {
    // wait for serial port to connect. Needed for native USB
  }

  Serial.write(11); // tell the host we're ready
  while (Serial.available() == 0) {
    // wait for the host to give us some data
  }
  fetchNewData();
}

void loop() {
  fetchNewData(); // check if the host has sent any updates
  currBlockHeight();
  delay(sleepTime);
  currPriceBTC();
  delay(sleepTime);
  currPriceSats();
  delay(sleepTime);
  currSupply();
  delay(sleepTime);
  blocksTillHalving();
  delay(sleepTime);
}

void fetchNewData() {
  if (Serial.available() > 0) {
    delay(100); // wait a bit just in case line is slow
    Serial.write(2); // we got the data
    height = Serial.readStringUntil('H');
    btcPrice = Serial.readStringUntil('P');
    satsPrice = Serial.readStringUntil('S');
    halvingBlocks = Serial.readStringUntil('B');
    coinsInCirculation = Serial.readStringUntil('\n');
  }
  // no new data, continue
}

void currBlockHeight() {
  lcd.home();
  lcd.clear();
  lcd.print("Chain Height:");
  lcd.setCursor(0, 1);
  lcd.print(height);
}

void currPriceBTC() {
  lcd.home();
  lcd.clear();
  lcd.print("BTC Price:");
  lcd.setCursor(0, 1);
  lcd.print("$" + btcPrice);
}

void currPriceSats() {
  lcd.home();
  lcd.clear();
  lcd.print("Sats/1USD:");
  lcd.setCursor(0, 1);
  lcd.write(byte(1));
  lcd.print(satsPrice);
}

void currSupply() {
  lcd.home();
  lcd.clear();
  lcd.print("Supply:");
  lcd.setCursor(0, 1);
  lcd.write(byte(0));
  lcd.print(coinsInCirculation);
}

void blocksTillHalving() {
  lcd.home();
  lcd.clear();
  lcd.print("Halving in:");
  lcd.setCursor(0, 1);
  lcd.print(halvingBlocks + " blocks");
}