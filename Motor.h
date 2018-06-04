#ifndef Motor_h
#define Motor_h
 
#include "Arduino.h"
 
class Motor
{
  public:
    Motor(int motorPinB, int motorPinF, int motorPinPWM, int speedCtrlPin);
    void CW();
    void CCW();
	  void STOP();
	  void SPEED();
  private:
    int _motorPinB;
	  int _motorPinF;
	  int _motorPinPWM;
	  int _speedCtrlPin;
	  int _sp =0;
};
 

#endif





