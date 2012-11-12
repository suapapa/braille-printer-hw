
#define PIN_LED 13

// pins for stepper
#define PIN_STP_1 14
#define PIN_STP_2 15
#define PIN_STP_3 16
#define PIN_STP_4 17

// pins for servos
#define PIN_SRV_1 9
#define PIN_SRV_2 10
#define PIN_SRV_3 11

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

#include <Servo.h>

Servo servoPunch[3];

static void _punchSetup(void)
{
  servoPunch[0].attach(PIN_SRV_1);
  servoPunch[1].attach(PIN_SRV_2);
  servoPunch[2].attach(PIN_SRV_3);
}

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

static void _printBraille(unsigned char ch)
{
  //Serial.println(ch);
  //pass
}

void setup(void)
{
  Serial.begin(9600);
  Serial.println("Hello");

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);

  stepperFeed.setSpeed(30);
  _punchSetup();
  _punch(0);

  digitalWrite(PIN_LED, LOW);
}

static int loopCnt = 0;

void loop(void)
{
  /*// Protocol begin with "####"*/
  /*if (Serial.available() > 4 &&*/
  /*    Serial.read() == '#' && Serial.read() == '#' &&*/
  /*    Serial.read() == '#' && Serial.read() == '#') {*/
  /*  unsigned char pktSize = Serial.read();*/
  /*  delay(300); // in 9600bps 256 bytes will come in 210ms.*/
  /*  for (int i = 0; i < pktSize; i++) {*/
  /*    _printBraille(Serial.read());*/
  /*  }*/
  /*  Serial.println("OK");*/
  /*}*/

  digitalWrite(PIN_LED, HIGH);
  loopCnt += 1;
  if (loopCnt < 100) {
    _feed(10);
  }
  delay(500);

  digitalWrite(PIN_LED, LOW);
  delay(500);

}

/* vim: set sw=2 et: */
