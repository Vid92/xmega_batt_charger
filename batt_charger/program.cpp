#include "program.h"
#include <Arduino.h>


void Program::runStep(){
  this->state0 = 1;
  //state0 = 1;
  //stepState = 'R';
  digitalWrite(LED_BUILTIN, HIGH); //E1

  digitalWrite(LedComms, HIGH); Serial1.write(2); Serial1.print(myaddress); Serial1.write(51); Serial1.write("ACTION: PASS,RUN"); Serial1.write(3); Serial1.write(0);Serial1.write(0); Serial1.write(4);delay(2); digitalWrite(LedComms, LOW);
  Ttime=0;
  if((type[count][0]) == 'E'){
    count = 0;
  }
}

void Program::pauseStep(){
  this->state0 = 2;
  //stepState = 'P';

  digitalWrite(LedComms, HIGH); Serial1.write(2); Serial1.print(myaddress); Serial1.write(52); Serial1.write("ACTION: PASS,PAUSE"); Serial1.write(3); Serial1.write(0); Serial1.write(0); Serial1.write(4);delay(2); digitalWrite(LedComms, LOW);
}

void Program::stopStep(){
  this->state0 = 3;

  digitalWrite(LedComms, HIGH);Serial1.write(2);Serial1.print(myaddress); Serial1.write(53); Serial1.write("ACTION: PASS,STOP"); Serial1.write(3); Serial1.write(0); Serial1.write(0); Serial1.write(4); delay(2); digitalWrite(LedComms, LOW);
}

void Program::nextStep(){
  this->state0 = 1;
  //state0 = 1;
  count = count + 1;

  if(count>16) count = 0;
}

void Program::process_step()
{
  if(this->state0 == 1)
  {
    Debug.println("entro-program");
    //Debug.print(type[count][0]);
    inicio:
    switch (type[count][0]){
      case 'B':  //Begin
      letter = "B";
      count = count + 1;
      Debug.println("Step-Begin");
      goto inicio;
      break;

      case 'C':  //Charge
      letter = "Charge"; //C
      Debug.println("Step-Charge");
      control.setCurrent(current[count]);
      control.setTime(duration[count]);
      control.setAmpHour(AmperH[count]);
      control.setTemperature(maxtemp[count],mintemp[count]);
      control.run();
      break;

      case 'P':  //Pause
      letter = "Pause";
      Debug.println("Step-Pause");
      control.stepPause(duration[count]);
      control.runPause();
      break;

      case 'E':  //End
      letter = "End";
      Ttime = 0;
      totAH = 0.0;
      control.stop();
      stepState = 'E';
      Debug.println("Step-End");
      break;
      default:break;
    }
    this->state0=4;
  }

  if(control.states())
  {
    this->state0 = 3;
  }

  if(this->prevstate0!=3 && this->state0 == 3)
  //if(prevstate0!=3 && state0 == 3)
  {
    control.stop();
    Ttime=0;
    count = 0;
    totAH = 0.0;
    letter = "Off";
  }

  if(this->prevstate0!= 2 && this->state0 == 2)
  //if(prevstate0!=2 && state0 == 2)
  {
     control.pause();
  }

  this->prevstate0 = this->state0;
  //prevstate0 = state0;
}
