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
  //Debug.println("Relay-on");
  if((this->prevstate == 0 || this->prevstate == 3 || this->prevstate == 4) && this->state == 1){
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

  if((this->prevstate == 0 || this->prevstate == 3) && this->state == 4){
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
          long averageCurrent = 0;
          long averageVoltage = 0;
          long averageTemp  = 0;

          for(int i = 0; i < 40; i++)
          {
            averageCurrent = averageCurrent + analogRead(A0);
            averageVoltage = averageVoltage + analogRead(A1);
            averageTemp = averageTemp + analogRead(A2);
          }
          averageCurrent = averageCurrent / 40;
          averageVoltage = averageVoltage / 40;
          averageTemp = averageTemp / 40;

          valcurrent = averageCurrent * 35.0 / 1023.0; //35 shunt
          valvoltage = averageVoltage * 500.0 / 1023.0;
          valtemp = averageTemp * 80.0 / 1023.0;
          //Serial.println(valcurrent);
          //Serial.println(valvoltage);
          //Serial.println(valtemp);

          //current0 = this->valcurrent;
          //voltage0 = this->valvoltage;
          //temperature0 = this->valtemp;

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
          this->state = 3;
          flagStep=true;
          //Debug.print("Relay-off");
          digitalWrite(LedRelay, LOW);
        }
      }
  }

  if(this->state == 4)
  {
    dac.write(0xFFF);
    if(controlTime.isRunning()){

      if(controlTime.ms() < this->steptime){
      }
      else
      {
        Debug.println("stepPause out");
        this->state = 3;
        flagStep=true;
      }
    }
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
      //Debug.print("Relay-offS");
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
      //Debug.print("Relay-offP");
      digitalWrite(LedRelay, LOW);
  }
  if(this->prevstate == 2 && (this->state == 1 || this->state == 4))
  {
    controlTime.play();
  }

  this->prevstate = this->state;
}
