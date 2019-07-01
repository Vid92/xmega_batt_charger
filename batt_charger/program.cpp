#include "program.h"
#include <Arduino.h>

void Program::runStep(){
  this->state0 = 1;
  digitalWrite(LED_BUILTIN, HIGH);

  digitalWrite(LedComms, HIGH); Serial1.write(2); Serial1.print(myaddress); Serial1.write(51); Serial1.write("ACTION: PASS"); Serial1.write(3); Serial1.write(0);Serial1.write(0); Serial1.write(4);delay(2); digitalWrite(LedComms, LOW);

  if((type[count][0]) == 'E'){
    count = 0;
  }
}

void Program::pauseStep(){
  this->state0 = 2;
  digitalWrite(LedComms, HIGH); Serial1.write(2); Serial1.print(myaddress); Serial1.write(52); Serial1.write("ACTION: PASS"); Serial1.write(3); Serial1.write(0); Serial1.write(0); Serial1.write(4);delay(2); digitalWrite(LedComms, LOW);
}

void Program::stopStep(){
  this->state0 = 3;
  digitalWrite(LedComms, HIGH);Serial1.write(2);Serial1.print(myaddress); Serial1.write(53); Serial1.write("ACTION: PASS"); Serial1.write(3); Serial1.write(0); Serial1.write(0); Serial1.write(4); delay(2); digitalWrite(LedComms, LOW);
}

void Program::nextStep(){
  this->state0 = 1;
  count = count + 1;

  if(count>14) count = 0;
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
      letter = 'B';
      count = count + 1;
      Debug.println("Step-Begin");
      goto inicio;
      break;

      case 'C':  //Charge
      letter = 'C';
      Debug.println("Step-Charge");
      control.setTime(duration[count]);
      control.setCurrent(current[count]);
      control.run();
      break;

      case 'P':  //Pause
      letter = 'P';
      Debug.println("Step-Pause");
      control.stepPause(duration[count]);
      control.runPause();
      break;

      case 'E':  //End
      letter = 'E';
      control.stop();
      Debug.println("Step-End");
      break;
      default:break;
    }
    this->state0=4;
  }

  if(this->prevstate0!=3 && this->state0 == 3)
  {
    control.stop();
    count = 0;
    letter = {0};
  }

  if(this->prevstate0!= 2 && this->state0 == 2)
  {
     control.pause();
  }

  this->prevstate0 = this->state0;
}
