/*
  braille-label-printer.ino - Arduino sketch for Braille Label Printer
  Copyright (c) 2012 Homin Lee <homin.lee@suapapa.net>. All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#define PIN_LED 13

// pins for LCD
#define PIN_LCD_RS 12
#define PIN_LCD_EN 11
#define PIN_LCD_D4 10
#define PIN_LCD_D5 9
#define PIN_LCD_D6 8
#define PIN_LCD_D7 7

// pins for stepper
#define PIN_STP_1 14
#define PIN_STP_2 15
#define PIN_STP_3 16
#define PIN_STP_4 17

// pins for servos
#define PIN_SRV_1 6
#define PIN_SRV_2 5
#define PIN_SRV_3 4

#include <LiquidCrystal.h>

LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_EN,
    PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);

static void _initLCD(void)
{
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Hello ");
  lcd.print("World");
}

#include <Stepper.h>

Stepper stepperFeed(200, PIN_STP_1, PIN_STP_2, PIN_STP_3, PIN_STP_4);

static void _turnOffStepper(void)
{
  digitalWrite(PIN_STP_1, LOW);
  digitalWrite(PIN_STP_2, LOW);
  digitalWrite(PIN_STP_3, LOW);
  digitalWrite(PIN_STP_4, LOW);
}

static void _feed(int step)
{
  stepperFeed.step(step);
  _turnOffStepper();
}

static void _initStepper(void)
{
  stepperFeed.setSpeed(30);
  _turnOffStepper();
}

#include <Servo.h>

Servo servoPunch[3];

static void _punch(int idx)
{
  Servo *servo;
  int press[3] = {90, 90, 90};
  int release[3] = {0, 0, 0};
  int msecs = 500;

  switch(idx) {
    case 1: case 2: case 3:
      servo = &servoPunch[idx - 1];
      servo->write(press[idx - 1]);
      delay(msecs);
      servo->write(release[idx - 1]);
      while(servo->read() != release[idx - 1]) {
        delay(10);
      }
      break;
    default:
      for (int i = 0; i < 3; i++) {
        servo = &servoPunch[i];
        servo->write(release[i]);
      }
      delay(1000);
      break;
  }
}

static void _initPunchs(void)
{
  servoPunch[0].attach(PIN_SRV_1);
  servoPunch[1].attach(PIN_SRV_2);
  servoPunch[2].attach(PIN_SRV_3);
  _punch(0);
}

void _displayBraille(int n, int t, unsigned char ch)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print('[');
  lcd.print(n);
  lcd.print('/');
  lcd.print(t);
  lcd.print(']');
  lcd.print("PRTing...");

  lcd.setCursor(1, 0);
  for (int i = 0; i < 8; i++) {
    if ((ch >> i) & 1) {
      lcd.print(i + 1);
      lcd.print('-');
    }
  }
}

static void _punchBraille(unsigned char ch)
{
  if ((ch >> 1) & 1)
    _punch(1);
  if ((ch >> 2) & 1)
    _punch(2);
  if ((ch >> 3) & 1)
    _punch(3);
  _feed(1);

  if ((ch >> 4) & 1)
    _punch(1);
  if ((ch >> 5) & 1)
    _punch(2);
  if ((ch >> 6) & 1)
    _punch(3);
  _feed(1);
}

void setup(void)
{
  Serial.begin(9600);
  Serial.println("Hello");

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);

  _initLCD();
  _initPunchs();
  _initStepper();
  _punch(0);
}

static int loopCnt = 0;

void loop(void)
{
  digitalWrite(PIN_LED, LOW);
  // Protocol begin with "####"
  if (Serial.available() > 4 &&
      Serial.read() == '#' && Serial.read() == '#' &&
      Serial.read() == '#' && Serial.read() == '#') {
    digitalWrite(PIN_LED, HIGH);
    unsigned char strSize = Serial.read();
    delay(300); // in 9600bps 256 bytes will come in 210ms.
    for (int i = 0; i < strSize; i++) {
      unsigned char bch = Serial.read();
      _displayBraille(i+1, strSize, bch);
      _punchBraille(bch);
    }
    Serial.print("OK");
  }
  delay(10);
}

/* vim: set sw=2 et: */
