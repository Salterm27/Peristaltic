#include "Motor.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>  //PONER QUE t_CW Y t_CCW SE REINICIEN CUANDO APRETAS EL BOTON DE MENU

//Conexion de pines


#define pedal_PIN 0  //pedal_PIN debe ser salida digital
#define motorPinB 12
#define motorPinF 13
#define motorPinPWM 11
#define speedCtrlPin A0
#define encoderOutputA 2 // external interrupt
#define encoderOutputB 7
#define encoderSwitch 3 //external interrupt
#define modeSwitch 9 //cuidado con config pcint


//Declaracion de variables globales

Motor motor(motorPinB, motorPinF, motorPinPWM, speedCtrlPin);
LiquidCrystal_I2C lcd(0x27,16,2);


int menu = 0;  // 0 modo manual, 1 modo matriz, 2 modo patron.
int t_CW = 4; 
int t_CCW = 4;
int patronlisto = 0;
int t;
int t_ON;
int aState;
int aLastState;
int setTime = 1;
int aux = 0;

//Programa

void setup() 
{
  lcd.begin();
  lcd.backlight();
  pinMode(pedal_PIN,INPUT_PULLUP);
  pinMode (encoderOutputA,INPUT_PULLUP);
  pinMode (encoderOutputB,INPUT_PULLUP);
  pinMode (encoderSwitch,INPUT_PULLUP);

  
  aLastState = digitalRead(encoderOutputA);
  motor.STOP();

  
  attachInterrupt(digitalPinToInterrupt(encoderOutputA), encoder_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderSwitch), encoderSwitch_ISR, FALLING);
  //Pinchange interrupt en 9 (pcint 1)
  pinMode(modeSwitch, INPUT_PULLUP);
  PCMSK0 = 2;
  PCICR = 1;
  //end of pcint config
}

void loop()
{ 
  switch (menu) 
  {
    case 0:
      patronlisto = 0;
      modo_MANUAL();
      break;
    case 1:
      patronlisto = 0;
      modo_MATRIZ();
      break;
    case 2:
      modo_PATRON();
      break;
  }
}


ISR(PCINT0_vect)
{
  delay(150);
  t_CCW = 0;
  t_CW = 0;
  aux++;
  if(aux%2 == 0)
  {
    menu++;
    if(menu > 2)
      menu = 0; 
  }
}
/*------------------------------Modo Manual------------------------*/

void modo_MANUAL()
{
  lcd.setCursor(0,0);
  lcd.print("      MODO      ");
  lcd.setCursor(0,1);
  lcd.print("     MANUAL     ");

  motor.SPEED();

  if(!digitalRead(pedal_PIN))
  {
    motor.CW();
  }
  else
  {
    motor.STOP();
  }
}

/*------------------------------Modo Matriz------------------------*/

void modo_MATRIZ()
{ 
  lcd.setCursor(0,0);
  lcd.print("   MODO MATRIZ   ");
  lcd.setCursor(0,1);
  if((t_CW/2) < 10)
  {
    lcd.print("T+:  ");
    lcd.print(t_CW/2);
    lcd.print("s ");
  }
  else
  {
    lcd.print("T+: ");
    lcd.print(t_CW/2);
    lcd.print("s");
  }  
  lcd.setCursor(8,1);
  if((t_CCW/2) < 10)
  {
    lcd.print("T-:  ");
    lcd.print(t_CCW/2);
    lcd.print("s");
  }
  else
  {
    lcd.print("T-: ");
    lcd.print(t_CCW/2);
    lcd.print("s");
  } 
  if(!digitalRead(pedal_PIN))
  {
    t = millis();
    motor.CW();
    do
    {
      motor.SPEED();
      t_ON = millis() - t;
      t_ON = t_ON/1000;
    }while(!digitalRead(pedal_PIN) && (t_CW/2) > t_ON);
    motor.STOP();
    if(!digitalRead(pedal_PIN))
    {
      t = millis();
      motor.CCW();
      do
      {
        motor.SPEED();
        t_ON = millis() - t;
        t_ON = t_ON/1000;
      }while( !digitalRead(pedal_PIN) && (t_CCW/2) > t_ON);
      motor.STOP();
    } 
  }
}

void encoder_ISR()
{
  if(setTime)
  {
    aState = digitalRead(encoderOutputA); 
    if(aState != aLastState)
    {
      if(digitalRead(encoderOutputB) != aState)
        t_CCW++;
      else
        t_CCW--;
      
      if(t_CCW < 0)
        t_CCW = 0;
    }
    aLastState = aState;
  }
  else
  {
    aState = digitalRead(encoderOutputA); 
    if (aState != aLastState)
    {
      if (digitalRead(encoderOutputB) != aState) 
        t_CW ++;
      else
        t_CW --;
      
      if( t_CW < 0)
        t_CW = 0;
    }
    aLastState = aState;  
  }
}

void encoderSwitch_ISR()
{
  if(setTime)
    setTime = 0;
  else
    setTime = 1;
}

/*------------------------------Modo Patron------------------------*/

void modo_PATRON()
{
  if(!patronlisto)
  {
    modo_leerPATRON();
  }
  else 
  {
    modo_repetirPATRON();
  }
}


void modo_leerPATRON()
{
  lcd.setCursor(0,0);
  lcd.print("  MODO  PATRON  ");
  lcd.setCursor(0,1);
  lcd.print("    GRABANDO    ");

  if(!digitalRead(pedal_PIN))
  {
    delay(100);
    
    t = millis();
    motor.CW();
    
    while(!digitalRead(pedal_PIN))
    {
      motor.SPEED();
    }

    motor.STOP();
    t_CW = millis() - t;
    if(t_CW<1000)
      t_CW = 1000;
    delay(100);
    
    while(digitalRead(pedal_PIN)) 
    {
      motor.STOP();
    }

    delay(100);
    
    t = millis();
    motor.CCW();
    
    while(!digitalRead(pedal_PIN))
    {
      motor.SPEED();
    }
    
    motor.STOP();
    t_CCW = millis() - t;    
    if(t_CCW<1000)
     t_CCW = 1000;   
    
    lcd.setCursor(0,0);
    lcd.print("  MODO  PATRON  ");
    lcd.setCursor(0,1);
    lcd.print("T+: ");
    lcd.print(t_CW/1000);
    lcd.print("s");
    lcd.setCursor(6,1);
    lcd.print("  T-: ");
    lcd.print(t_CCW/1000);
    lcd.print("s");

  
    patronlisto = 1;
  }
}

void modo_repetirPATRON()
{
  if(!digitalRead(pedal_PIN))
  {
    t = millis();
    motor.CW();
    do
    {
      motor.SPEED();
      t_ON = millis() - t;      
    }while(!digitalRead(pedal_PIN) && t_CW > t_ON);
    motor.STOP();
    t_ON = 0;
    if(!digitalRead(pedal_PIN))
    {
      t = millis();
      motor.CCW();
      do
      {
        motor.SPEED();
        t_ON = millis() - t;
      }while( !digitalRead(pedal_PIN) && t_CCW > t_ON);
      motor.STOP();
    } 
  } 
}


