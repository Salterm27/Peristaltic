/*
  Resinadora.cpp
*/
 
#include "Arduino.h"
#include "Motor.h"
#define MaxSpeed12 255
#define MinSpeed12 60

 
Motor::Motor(int motorPinB, int motorPinF, int motorPinPWM, int speedCtrlPin)
{
  pinMode(motorPinF, OUTPUT);
  pinMode(motorPinB, OUTPUT);
  _motorPinB = motorPinB;
  _motorPinF = motorPinF;
  _motorPinPWM = motorPinPWM;
  _speedCtrlPin = speedCtrlPin;
}


void Motor::CW()
{
  digitalWrite(_motorPinF, HIGH);
  digitalWrite(_motorPinB, LOW);  
}

void Motor::CCW()
{
  digitalWrite(_motorPinF, LOW);
  digitalWrite(_motorPinB, HIGH);  
}

void Motor::STOP()
{
  digitalWrite(_motorPinF, LOW);
  digitalWrite(_motorPinB, LOW);
}

void Motor::SPEED()
{
  _sp = analogRead(_speedCtrlPin);
 _sp = map(_sp, 0, 1023, MinSpeed12, MaxSpeed12);  //MI MOTOR 3-6V DC
  analogWrite(_motorPinPWM,_sp);
}

