#include "control.h"
#include <Arduino.h>

void Control::begin(){
  int result=dac.begin(xmDAC::SINGLE_CHANNEL_MODE, xmDAC::VREF_VCC);
  dac.write(0xFFF);
}

void Control::setCurrent(float val_control){
  this->val_control = val_control;
}

void Control::setTime(unsigned long timeout){
  this->timeout = timeout * 1000;
}

void Control::setAmpHour(float valAmpHour){
  this->valAmpHour = valAmpHour;
}

void Control::setTemperature(float maxTemp,float minTemp){
  this->maxTemp = maxTemp;
  this->minTemp = minTemp;
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
}

void Control::stop() {
  this->state = 3;
  digitalWrite(LED_BUILTIN, LOW);
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
  this->steptime = steptime * 1000;
}


void Control::readData(){
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

  this->valcurrent0 = this->averageCurrent - 36.0;
  valcurrent = this->valcurrent0 * 35.0 / 1023.0;

  this->valAH = valcurrent * 0.000277 * controlTime.ms() * 0.001;

  //Debug.println(this->averageVoltage);
  this->valvoltage0 = this->averageVoltage - 44.0;
  valvoltage = this->valvoltage0 * 500.0 / 1023.0;

  //Debug.println(this->valvoltage0);
  valtemp = this->averageTemp * 120.0 / 1023.0;
}

void Control::event() {
  //here your logic to control the current
  if(this->state == 1)
  {
      if(controlTime.isRunning())
      {
        if(this->timeout!=0){
          //Debug.print("TIMEOUT");
          if(controlTime.ms() < this->timeout)
          {
              if(this->maxTemp!=0){
                if(valtemp > this->maxTemp){ //se va a pause
                  this->state=2; Debug.println("Pause-temp");
                  stepState = 'T';
                }
              }

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
            Ttime0 = Ttime;
            Ttime = Ttime0 + (controlTime.ms()*0.001);
            controlTime.stop();
            flagStep=true;
          }
        }
        else
        {
          //Debug.print("AH");
          if(this->valAH < this->valAmpHour)
          {
              if(this->maxTemp!=0){
                if(valtemp > this->maxTemp){ //se va a pause
                  this->state=2; Debug.println("Pause-temp");
                  stepState = 'T';
                }
              }

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
        Ttime0 = Ttime;
        Ttime = Ttime0 + (controlTime.ms()*0.001);
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

  //if((this->prevstate!= 2 || this->prevstate == 2)&&this->state == 2)
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
      digitalWrite(LedRelay, LOW); //solo digitalWrite
  }

  if(this->prevstate == 2 &&this->state == 2){
    if(valtemp <= minTemp){
      this->state = 1;
      stepState = 'R';
      Debug.println("good-temp");
    }
  }

  if(this->prevstate == 2 && (this->state == 1 || this->state == 4))
  {
    controlTime.play();
    if(this->state==4)digitalWrite(LedRelay, LOW);
    if(this->state==1)digitalWrite(LedRelay, HIGH);
  }

  this->prevstate = this->state;
}
