// This is just a mockup sketch for test scons works

#define PIN_LED 13
#define PIN_FEED_SRV_1 14
#define PIN_FEED_SRV_2 15
#define PIN_FEED_SRV_3 16
#define PIN_FEED_SRV_4 17

#include <Stepper.h>

Stepper feedStepper(100, 14, 15, 16, 17);

static void _feed()
{
  feedStepper.step(100);
  /*feedStepper.step(-100);*/
}

static void _printBraille(unsigned char ch)
{
  //Serial.println(ch);
  //pass
}

void setup(void)
{
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);

  feedStepper.setSpeed(60);

  Serial.begin(9600);
  Serial.println("Hello");
}

void loop(void)
{
  // Protocol begin with "####"
  if (Serial.available() > 4 &&
      Serial.read() == '#' && Serial.read() == '#' &&
      Serial.read() == '#' && Serial.read() == '#') {
    unsigned char pktSize = Serial.read();
    delay(300); // in 9600bps 256 bytes will come in 210ms.
    for (int i = 0; i < pktSize; i++) {
      _printBraille(Serial.read());
    }
    Serial.println("OK");
  }
}

/* vim: set sw=2 et: */
