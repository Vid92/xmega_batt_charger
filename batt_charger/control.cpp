#include "control.h"
#include <Arduino.h>

void Control::begin(){
  int result=dac.begin(xmDAC::SINGLE_CHANNEL_MODE, xmDAC::VREF_VCC);
  dac.write(0xFFF);
}

void Control::setCurrent(float val_control) {
  this->val_control = val_control;
}

void Control::setTime(unsigned long timeout) {
  this->timeout = timeout;
}

void Control::run() {
  this->state = 1;

  digitalWrite(LedRelay, HIGH);
  if((this->prevstate == 0 || this->prevstate == 3 || this->prevstate == 4 || this->prevstate == 1) && this->state == 1){
    controlTime.start();
  }
}

void Control::pause() {
  this->state = 2;
  valcurrent = 0;
  valvoltage = 0;
  valtemp = 0;
}

void Control::stop() {
  this->state = 3;
  digitalWrite(LED_BUILTIN, LOW);
  valcurrent = 0;
  valvoltage = 0;
  valtemp = 0;
}

void Control::runPause(){
  this->state = 4;

  valcurrent = 0;
  valvoltage = 0;
  valtemp = 0;

  while(this->valrampa > 0)
  {
    this->valrampa--;
    dac.write(0xFFF-this->valrampa);
    delay(1);
  }

  digitalWrite(LedRelay, LOW);

  if((this->prevstate == 0 || this->prevstate == 3 || this->prevstate == 1) && this->state == 4){
    controlTime.start();
  }
}

void Control::stepPause(unsigned long steptime){
  this->steptime = steptime;
}

void Control::event() {
  //here your logic to control the current

  if(this->state == 1)
  {
      if(controlTime.isRunning())
      {
        if(controlTime.ms() < this->timeout)
        {
          this->averageCurrent = 0;
          this->averageVoltage = 0;
          this->averageTemp  = 0;

          for(int i = 0; i < 40; i++)
          {
            this->averageCurrent = this->averageCurrent + analogRead(A0);
            this->averageVoltage = this->averageVoltage + analogRead(A1);
            this->averageTemp = this->averageTemp + analogRead(A2);
          }
          this->averageCurrent = this->averageCurrent / 40;
          this->averageVoltage = this->averageVoltage / 40;
          this->averageTemp = this->averageTemp / 40;

          valcurrent = this->averageCurrent * 35.0 / 1023.0;
          valvoltage = this->averageVoltage * 500.0 / 1023.0;
          valtemp = this->averageTemp * 80.0 / 1023.0;

          /*PORTD.OUTTGL=PIN1_bm;
          Debug.print("v1,");
          Debug.print(valcurrent);
          Debug.print("v2,");
          Debug.print(valvoltage);
          Debug.print("v3,");
          Debug.print(valtemp);*/

          //Serial.println(valcurrent);
          //Serial.println(valvoltage);
          //Serial.println(valtemp);

          if(valcurrent < this->val_control)
          {
            if (this->valrampa<0xFFF)
              this->valrampa++;
          }

          if(valcurrent > this->val_control)
          {
            if(this->valrampa > 0)
              this->valrampa--;
          }
          dac.write(0xFFF-this->valrampa);
          delay(1);
        }
        else
        {
          Debug.println("timeout-agotado");
          controlTime.stop();
          flagStep=true;
        }
      }
  }

  if(this->state == 4)
  {
    if(controlTime.isRunning())
    {
      if(controlTime.ms() > this->steptime)
      {
        //Debug.print("controlTime: "); Debug.println(controlTime.ms());
        Debug.println("stepPause out");
        //this->state = 3;
        flagStep=true;
      }
    }
      delay(1);
  }

  if(this->prevstate!=3 && this->state == 3)
  {
    Debug.println("STOP");
    controlTime.stop();
    while(this->valrampa > 0)
    {
      this->valrampa--;
      dac.write(0xFFF-this->valrampa);
      delay(1);
    }
    digitalWrite(LedRelay, LOW);
  }

  if(this->prevstate!= 2 && this->state == 2)
  {
     Debug.println("PAUSE");
     controlTime.pause();
      while(this->valrampa > 0)
      {
        this->valrampa--;
        dac.write(0xFFF-this->valrampa);
        delay(1);
      }
      digitalWrite(LedRelay, LOW);
  }
  if(this->prevstate == 2 && (this->state == 1 || this->state == 4))
  {
    controlTime.play();
    if(this->state==4)digitalWrite(LedRelay, LOW);
    if(this->state==1)digitalWrite(LedRelay, HIGH);
  }

  this->prevstate = this->state;
}
