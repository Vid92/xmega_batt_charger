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
  if(this->timeout!=0){
    if (this->timeout <= 60000) //diferenciar entre seg. o min-hor
    { //verificar!!!
      this->time1 = 500;
      this->time2 = 900;
    }
  }
}

void Control::setAmpHour(float valAmpHour){
  this->valAmpHour = valAmpHour;
  this->timeAH = this->valAmpHour / (this->val_control * 0.000277); //valor en seg
  if(this->timeAH !=0){
    if(this->timeAH <=60){
      this->time1 = 500; //valor ms
      this->time2 = 900;
    }
  }
}

void Control::setTemperature(float maxTemp,float minTemp){
  this->maxTemp = maxTemp;
  this->minTemp = minTemp;
}

void Control::run() {
  this->state = 1;

  digitalWrite(LedRelay, HIGH);
  if((this->prevstate == 0 || this->prevstate == 3 || this->prevstate == 4 || this->prevstate == 1) && this->state == 1){
    stepState = 'R';
    this->flagP = false;
    controlTime.start();
  }
}

void Control::pause() {
  this->state = 2;
  this->flagEnable = true;
}

void Control::stop() {
  this->state = 3;
  //digitalWrite(LED_BUILTIN, LOW);
}

void Control::runPause(){
  this->state = 4;

  valcurrent = 0;
  valvoltage = 0;
  valtemp = 0;

  controlTime.stop();
  while(this->valrampa > 0)
  {
    this->valrampa--;
    dac.write(0xFFF-this->valrampa);
    delay(1);
  }

  digitalWrite(LedRelay, LOW);

  if((this->prevstate == 0 || this->prevstate == 3 || this->prevstate == 1 || this->prevstate == 4) && this->state == 4){
    controlTime.start();
  }
}

void Control::stepPause(unsigned long steptime){
  this->steptime = steptime * 1000;
}

bool Control::states(){
  return this->flagS;
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

  this->valcurrent0 = this->averageCurrent - 70.0; //36
  valcurrent = this->valcurrent0 * 35.0 / 1023.0;

  valAH = valcurrent * 0.000277 * controlTime.ms() * 0.001;

  //Debug.println(this->averageVoltage);
  this->valvoltage0 = this->averageVoltage - 44.0;
  valvoltage = this->valvoltage0 * 500.0 / 1023.0;

  //Debug.println(this->valvoltage0);
  valtemp = this->averageTemp * 120.0 / 1023.0;
  //Debug.print("readTemp:");
  //Debug.println(valtemp);
  delay(1);
}

void Control::event() {
  //here your logic to control the current
  if(this->state == 1)
  {
      if(controlTime.isRunning())//Program running
      {
        if(this->timeout!=0){ //Running with time
          //Debug.print("TIMEOUT");
          if(controlTime.ms() < this->timeout)
          {
              if(this->flagTemp != false){
                this->flagTemp = false;
                this->t2 = controlTime.ms() + 30000;
                Debug.print("t2:");
                Debug.println(this->t2);
              }
              //--------------- control current -----------------//
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
             //---------------- current Warning ------------------//
             if(controlTime.ms() > this->time1 && controlTime.ms() > this->t2)//toma una muestra cada 1.2s
             {
                Debug.println("save0");
                this->time1 = this->time1 + this->time2;
                this->tmpVal = valcurrent - 3; //guarda valor temp
                if (this->tmpVal<=0 || this->tmpVal>=valcurrent)
                  stepState = 'W';
             }

             if(this->tmpVal<=0 && controlTime.ms() > this->time2 && controlTime.ms() > this->t2)
             {
               this->flagO = true;
               Debug.print("tmval0");
               if (this->flagO !=false){
                 stepState = 'W';
                 this->flagS = true; //flag manda a state = Stop
                 Debug.println("dentro");
               }
             }

             else if(this->tmpVal>=valcurrent && controlTime.ms() > this->time2 && controlTime.ms() > this->t2)
             {
               if (this->flagO !=true){
                 stepState = 'W';
                 this->flagO = true;
                 this->t1 = controlTime.ms() + 20000;
                 Debug.print("T1me:");
                 Debug.println(this->t1);
               }
               else if(controlTime.ms() > this->t1 && this->flagO != false){
                 this->flagS = true;
                 Debug.print("ElseTime:");
                 Debug.println(controlTime.ms());
               }
             }
             //----------------- control temp -------------------//
             else if(this->maxTemp!=0){
               if(valtemp > this->maxTemp){ //se va a pause
                 this->state = 2; Debug.println("Pause-temp");
                 stepState = 'T';
                 this->flagEnable = true;
                 this->flagTemp = true;
               }
             }
          }
          else
          {
            Debug.println("timeout-agotado");
            this->t1 = 0;
            this->t2 = 0;
            this->time1 = 60000;
            this->time2 = 90000;
            this->tmpVal = 0;
            this->Ttime0 = Ttime;
            Ttime = this->Ttime0 + (controlTime.ms()*0.001);
            controlTime.stop();
            flagStep=true;
          }
        }
        else
        {
          //running with Amp-hour  //Debug.print("AH");
          if(valAH < this->valAmpHour)
          {
             if(this->flagTemp != false){
               this->flagTemp = false;
               this->t2 = controlTime.ms() + 30000;
               Debug.print("t2:");
               Debug.println(this->t2);
             }
             //-------------- control current ---------------//
             if(valcurrent < this->val_control)
             {
               if(this->valrampa<0xFFF)
                 this->valrampa++;
             }

             if(valcurrent > this->val_control)
             {
               if(this->valrampa > 0)
                 this->valrampa--;
             }
             dac.write(0xFFF-this->valrampa);
             delay(1);

             //---------------- current Warning ----------------//
             if(controlTime.ms() > this->time1 && controlTime.ms() > this->t2)
             {
               Debug.print("save1");
               this->time1 = this->time1 + this->time2;
               this->tmpVal = valcurrent - 1; //guarda valor temp
               if (this->tmpVal<=0 || this->tmpVal>=valcurrent)
                 stepState = 'W';
             }

             if(this->tmpVal<=0 && controlTime.ms() > this->time2 && controlTime.ms() > this->t2)
             {
               this->flagO = true;
               Debug.print("tmval1");
               if (this->flagO !=false){
                 stepState = 'W';
                 this->flagS = true; //flag manda a state = Stop
                 Debug.println("dentro2");
               }
             }

             else if(this->tmpVal>=valcurrent && controlTime.ms() > this->time2 && controlTime.ms() > this->t2)
             {
               if (this->flagO !=true){
                 stepState = 'W';
                 this->flagO = true;
                 this->t1 = controlTime.ms() + 20000;
                 Debug.print("T1me2:");
                 Debug.println(this->t1);
               }
               else if(controlTime.ms() > this->t1 && this->flagO != false){
                 this->flagS = true;
                 Debug.print("ElseTime2:");
                 Debug.println(controlTime.ms());
               }
             }

             //---------------- control Temp ----------------//
             if(this->maxTemp!=0){
               if(valtemp > this->maxTemp){ //se va a pause
                 this->state = 2; Debug.println("Pause-temp");
                 stepState = 'T';
                 this->flagEnable = true;
                 this->flagTemp = true;
               }
             }
          }
          else
          {
            Debug.println("timeout-agotado");
            this->t1 = 0;
            this->t2 = 0;
            this->time1 = 30000;
            this->time2 = 50000;
            this->tmpVal = 0;
            this->Ttime0 = Ttime;
            Ttime = this->Ttime0 + (controlTime.ms()*0.001);
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
        flagStep=true;
        this->Ttime0 = Ttime;
        Ttime = this->Ttime0 + (controlTime.ms()*0.001);
      }
    }
      delay(1);
  }

  /*
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
    stepState = 'S';
  }*/

  if(this->state == 3)
  {
    if(this->prevstate != 3){
      this->time1 = 60000;
      this->tmpVal = 0;
      this->t1 = 0;
      this->t2 = 0;
      this->flagPause = false;
      this->flagEnable = false;

      controlTime.stop();
      Debug.println("STOP");
    }
    if(this->flagS != false){
      dac.write(0xFFF);
      delay(1);
      this->flagO = false;
      this->flagS = false;
      this->valrampa = 0;
      Debug.println("flagS");
    }
    if(this->valrampa > 0)
    {
      this->valrampa--;
      dac.write(0xFFF-this->valrampa);
      delay(1);
    }
    else{
      this->state = 0;
      stepState = 'S';
      digitalWrite(LedRelay, LOW);
      digitalWrite(LED_BUILTIN, LOW);
    }
  }

  /*if(this->prevstate!= 2 && this->state == 2)
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
      stepState = 'P';
  }*/

  if(this->state == 2 && this->flagEnable !=false)
  {
    if(this->prevstate!= 2){
      Debug.println("PAUSE");
      controlTime.pause();
      if(this->flagTemp != false){
        Debug.println("flagPause");
        this->flagPause = true;
        this->flagP = false;
      }
      else{
        this->flagP = true;
      }
    }
    if(this->valrampa > 0)
    {
      this->valrampa--;
      dac.write(0xFFF-this->valrampa);
      delay(1);
    }
    else{
      digitalWrite(LedRelay, LOW); //solo digitalWrite
      stepState = 'P';
      this->flagPause = false;
      this->flagEnable = false;
    }
  }

  if(this->prevstate == 2 &&this->state == 2 && this->flagPause !=true && this->flagP != true){
    if(valtemp <= minTemp){
      this->state = 1;
      stepState = 'R';
      Debug.println("good-temp");
    }
  }

  if(this->prevstate == 2 && (this->state == 1 || this->state == 4))
  {
    controlTime.play();
    stepState = 'R';
    if(this->state==4)digitalWrite(LedRelay, LOW);
    if(this->state==1)digitalWrite(LedRelay, HIGH);
  }

  this->prevstate = this->state;
}
