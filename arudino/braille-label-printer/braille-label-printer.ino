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

// #define FACTORY_TEST

#define PIN_LED 13

// pins for LCD
#define PIN_LCD_RS 9
#define PIN_LCD_EN 8
#define PIN_LCD_D4 16
#define PIN_LCD_D5 17
#define PIN_LCD_D6 18
#define PIN_LCD_D7 19
#define PIN_LCD_BL 11

// pins for stepper
#define PIN_STP_1 6
#define PIN_STP_2 7
#define PIN_STP_3 10
#define PIN_STP_4 12

// pins for servos
#define PIN_SRV_1 5
#define PIN_SRV_2 2
#define PIN_SRV_3 4

/*---------------------------------------------------------------------------*/

#include <LiquidCrystal.h>

LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_EN,
    PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);

void _displayBraille(int n, int t, unsigned char ch)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print('[');
  lcd.print(n);
  lcd.print('/');
  lcd.print(t);
  lcd.print("] ");

  for (int i = 0; i < 6; i++) {
    if ((ch >> i) & 1)
      lcd.print(i + 1);
  }
}

static int lastBL = 0;
static int _lcdBacklight(int v)
{
  if (lastBL == v)
    return 0;

  int delayed = 0;

  if (lastBL > v) {
    for (int i = lastBL; i != v; i--) {
      analogWrite(PIN_LCD_BL, i);
      delay(10);
      delayed += 10;
    }
  } else {
    for (int i = lastBL; i != v; i++) {
      analogWrite(PIN_LCD_BL, i);
      delay(10);
      delayed += 10;
    }
  }
  lastBL = v;

  return delayed;
}

static void _initLCD(void)
{
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Braille Label");
  lcd.setCursor(5, 1);
  lcd.print("Printer");
  _lcdBacklight(200);
}

/*---------------------------------------------------------------------------*/

#include <Stepper.h>

Stepper stepperFeed(200, PIN_STP_3, PIN_STP_4, PIN_STP_2, PIN_STP_1);

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

/*---------------------------------------------------------------------------*/

#include <Servo.h>

#define SECS 1000

Servo servoPunch[3];

static void _press(int idx)
{
  Servo *servo;
  int press[3] = {30, 0, 180};

  switch(idx) {
    case 1: case 2: case 3:
      digitalWrite(PIN_LED, HIGH);
      servo = &servoPunch[idx - 1];
      servo->write(press[idx - 1]);
      break;
  }
}

static void _release(int idx)
{
  Servo *servo;
  int release[3] = {120, 120, 60};

  switch(idx) {
    case 1: case 2: case 3:
      digitalWrite(PIN_LED, LOW);
      servo = &servoPunch[idx - 1];
      servo->write(release[idx - 1]);
      break;
  }
}

static void _pressAndRelease(int idx)
{
  switch(idx) {
    case 1: case 2: case 3:
      lcd.setCursor(0, 1);
      lcd.print("Puncing ");
      lcd.print(idx);
      lcd.print("...");

      _press(idx); delay(2 * SECS);
      _release(idx); delay(0.5 * SECS);
      break;

    default:
      _release(1);
      _release(2);
      _release(3);
      delay(1 * SECS);
      break;
  }
}

static void _initPunchs(void)
{
  servoPunch[0].attach(PIN_SRV_1);
  servoPunch[1].attach(PIN_SRV_2);
  servoPunch[2].attach(PIN_SRV_3);
  _pressAndRelease(0);
}

static void _punchBraille(unsigned char ch)
{
  _feed(10);

  if ((ch >> 0) & 1)
    _pressAndRelease(1);
  if ((ch >> 1) & 1)
    _pressAndRelease(2);
  if ((ch >> 2) & 1)
    _pressAndRelease(3);
  _feed(5);

  if ((ch >> 3) & 1)
    _pressAndRelease(1);
  if ((ch >> 4) & 1)
    _pressAndRelease(2);
  if ((ch >> 5) & 1)
    _pressAndRelease(3);
}

/*---------------------------------------------------------------------------*/

#define SERIAL_SPEED 9600

void setup(void)
{
  Serial.begin(SERIAL_SPEED);

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);

  _initLCD();
  _initPunchs();
  _initStepper();
  _feed(300);
  _pressAndRelease(0);
}

void loop(void)
{
  digitalWrite(PIN_LED, LOW);
  // Protocol begin with "####"
  if (Serial.available() > 4 &&
      Serial.read() == '#' && Serial.read() == '#' &&
      Serial.read() == '#' && Serial.read() == '#') {
    unsigned char strSize = Serial.read();
    lcd.clear();
    lcd.setCursor(0,0);
#ifdef FACTORY_TEST
    delay(strSize * 1000 / (SERIAL_SPEED / 8) + 10);
    unsigned char testFunction = Serial.read();
    switch (testFunction) {
      case 'P':
        _press(Serial.read() - '0');
        break;
      case 'R':
        _release(Serial.read() - '0');
        break;
      case 'F':
        _feed(100);
        break;
    }
#else
    lcd.print("RCVing ");
    lcd.print(strSize);
    lcd.print(" chars...");
    _lcdBacklight(200);
    delay(strSize * 1000 / (SERIAL_SPEED / 8) + 10);
    for (int i = 0; i < strSize; i++) {
      unsigned char bch = Serial.read();
      _displayBraille(i+1, strSize, bch);
      _punchBraille(bch);
    }
#endif
    // Serial.print("OK");
  }
  lcd.clear();
  lcd.setCursor(0,0);
#ifdef FACTORY_TEST
  lcd.print("# FACTORY TEST #");
#else
  lcd.print("Wating...");
#endif

  if (0 == _lcdBacklight(100))
    delay(10);
}

/* vim: set sw=2 et: */
