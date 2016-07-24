//written by ian macfarlane for TMIT research and dev.
//temple university
//fall 2015
//libraries included in google site for ideas2

// motor drive init
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *leftMotor = AFMS.getMotor(4);
Adafruit_DCMotor *rightMotor = AFMS.getMotor(2);
// end motor drive init

// encoder init
int leftVal;
int leftEncoderPinA = 2;
int leftEncoderPinB = 4;
int leftEncoderPos = 0;
int leftEncoderPinALast = LOW;
int leftN = LOW;
int rightVal;
int rightEncoderPinA = 3;
int rightEncoderPinB = 5;
int rightEncoderPos = 0;
int rightEncoderPinALast = LOW;
int rightN = LOW;
// end encoder init

// button init
#include <ButtonV2.h>
ButtonV2 Button1;
const byte ButtonPin = 8;
// end button init

// direction LED init
const byte upLedPin = 9;
const byte rightLedPin = 10;
const byte downLedPin = 11;
const byte leftLedPin = 12;
//volatile int previousLed = leftLedPin;
volatile byte currentLed = upLedPin;
int upLedState = HIGH;
int rightLedState = LOW;
int downLedState = LOW;
int leftLedState = LOW;
// end LED init

// movement scheme init
const byte puckDir = upLedPin;
const int degTurn90 = 20;   //ticks on encoder
const int degTurn180 = 2*degTurn90; //ticks on encoder
// end movement scheme init

// function finds how much to turn based on current direction and desired direction
int howMuchToTurn (int puck_dir, int desired_dir) {
  int turn_code = puck_dir - desired_dir;
  switch (abs(turn_code)) {
    case 0:
      return 0;
      break;
    case 1:
      return degTurn90;
      break;
    case 2:
      return degTurn180;
      break;
    case 3:
      return degTurn90;
      break;
  }
}

int setMotorDir (int puck_dir, int desired_dir) {
  int turn_code = puck_dir - desired_dir;
  switch (abs(turn_code)) {
    case 0:
      leftMotor->run(FORWARD);
      rightMotor->run(FORWARD);
      break;
    case 1:
      leftMotor->run(FORWARD);
      rightMotor->run(BACKWARD);
      break;
    case 2:
      leftMotor->run(FORWARD);
      rightMotor->run(BACKWARD);
      break;
    case 3:
      leftMotor->run(BACKWARD);
      rightMotor->run(FORWARD);
      break;
  }
}

void encoderTrack() {
   leftN = digitalRead(leftEncoderPinA);
   if ((leftEncoderPinALast == LOW) && (leftN == HIGH)) {
     if (digitalRead(leftEncoderPinB) == LOW) {
       leftEncoderPos--;
     } else {
       leftEncoderPos++;
     }
   } 
   leftEncoderPinALast = leftN;

   rightN = digitalRead(rightEncoderPinA);
   if ((rightEncoderPinALast == LOW) && (rightN == HIGH)) {
     if (digitalRead(rightEncoderPinB) == LOW) {
       rightEncoderPos--;
     } else {
       rightEncoderPos++;
     }
   }
   rightEncoderPinALast = rightN;
}

byte ledSwitch(byte current_dir) {
  switch (current_dir) {
    case 9:
      //currentLed = 10;
      digitalWrite(upLedPin, LOW);
      digitalWrite(downLedPin, LOW);
      digitalWrite(leftLedPin, LOW);
      return 10;
      break;
    case 10:
      //currentLed = 11;
      digitalWrite(upLedPin, LOW);
      digitalWrite(rightLedPin, LOW);
      digitalWrite(leftLedPin, LOW);
      return 11;
      break;
    case 11:
      //currentLed = 12;
      digitalWrite(upLedPin, LOW);
      digitalWrite(downLedPin, LOW);
      digitalWrite(rightLedPin, LOW);
      return 12;
      break;
    case 12:
      //currentLed = 9;
      digitalWrite(leftLedPin, LOW);
      digitalWrite(downLedPin, LOW);
      digitalWrite(rightLedPin, LOW);
      return 9;
      break;
    }
}

void setup() {
  //button setup
  pinMode(ButtonPin, INPUT_PULLUP);
  Button1.SetStateAndTime(LOW);
  // end button setup

  //motor drive setup
  AFMS.begin();
  leftMotor->setSpeed(75);
  rightMotor->setSpeed(75);
  // end motor drive setup

  //encoder setup
  pinMode(leftEncoderPinA,INPUT);
  pinMode(leftEncoderPinB,INPUT);
  pinMode(rightEncoderPinA, INPUT);
  pinMode(rightEncoderPinB, INPUT);
  // end encoder setup

  //no movement scheme setup needed
  //end 

  Serial.begin(9600);
}

void loop() {

  digitalWrite(7, HIGH);
  byte type = Button1.CheckButton(ButtonPin); // current time and length of time to press
                                              // the button as many times as you can ie. 1.5 seconds
   
  switch (type)
  {
    case WAITING:
      // default idle settings reside here. this case is for when the button is released
      leftMotor->run(RELEASE);
      rightMotor->run(RELEASE);

      //digitalWrite(previousLed, LOW);
      digitalWrite(currentLed, HIGH);
      break;
    case PRESSED:
      // set previousLED to what currentLED was
      //previousLed = currentLed;
      // set currentLED to next LED (in clockwise fashion)
      currentLed = ledSwitch(currentLed);
    case DOUBLE_PRESSED:
      break;
    case MULTI_PRESSED:
      break;
    case HELD:
      while (currentLed != puckDir){
        byte turnDist = howMuchToTurn(puckDir, currentLed);
        //Serial.println(turnDist);
        while (abs(leftEncoderPos) < abs(turnDist)) {
          encoderTrack();
          setMotorDir(puckDir, currentLed);
          //Serial.println(abs(leftEncoderPos));
        }
        leftMotor->run(RELEASE);
        rightMotor->run(RELEASE);
        leftEncoderPos = 0;
        rightEncoderPos = 0;
        currentLed = ledSwitch(12);
      }
      while (currentLed == puckDir) {
        while (abs(leftEncoderPos) < 100) {
          encoderTrack();
          leftMotor->run(FORWARD);
          rightMotor->run(FORWARD);
        }
        leftMotor->run(RELEASE);
        rightMotor->run(RELEASE);
        leftEncoderPos = 0;
        rightEncoderPos = 0;
        break;
      }
 
      //delay(500);
      break;
    }
}
