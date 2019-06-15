#include "program.h"
#include <Arduino.h>

void Program::runStep(){
  this->state0 = 1;
  digitalWrite(LED_BUILTIN, HIGH);

  unsigned char run2[] = {"ACTION: PASS"};

  int dato = crc16_SingleBuf(run2,11);
  int crc16_high = highByte(dato);
  int crc16_low = lowByte(dato);

  Debug.print(crc16_low);Debug.println(crc16_high);

  digitalWrite(LedComms, HIGH); Serial1.write(2); Serial1.print(myaddress); Serial1.write(51); Serial1.write("ACTION: PASS"); Serial1.write(3); Serial1.write(crc16_low);Serial1.write(crc16_high); Serial1.write(4);delay(2); digitalWrite(LedComms, LOW);

  if((type[count][0]) == 'E'){
    count = 0;
  }
}

void Program::pauseStep(){
  this->state0 = 2;

  unsigned char pauseStep2[] = {"ACTION: PASS"};

  int dato = crc16_SingleBuf(pauseStep2,11);
  int crc16_high = highByte(dato);
  int crc16_low = lowByte(dato);

  Debug.print(crc16_low);Debug.println(crc16_high);
  digitalWrite(LedComms, HIGH); Serial1.write(2); Serial1.print(myaddress); Serial1.write(52); Serial1.write("ACTION: PASS"); Serial1.write(3); Serial1.write(crc16_low); Serial1.write(crc16_high); Serial1.write(4);delay(2); digitalWrite(LedComms, LOW);
}

void Program::stopStep(){
  this->state0 = 3;
  unsigned char stopStep2[] = {"ACTION: PASS"};

  int dato = crc16_SingleBuf(stopStep2,11);
  int crc16_high = highByte(dato);
  int crc16_low = lowByte(dato);

  Debug.print(crc16_low); Debug.println(crc16_high);

  digitalWrite(LedComms, HIGH);Serial1.write(2);Serial1.print(myaddress); Serial1.write(53); Serial1.write("ACTION: PASS"); Serial1.write(3); Serial1.write(crc16_low); Serial1.write(crc16_high); Serial1.write(4); delay(2); digitalWrite(LedComms, LOW);
}

void Program::nextStep(){
  this->state0 = 1;
  count = count + 1;

  if(count>14) count = 0;
  //this->count = count + 1;    //hasta 15 step max
  //if(this->count>14)  this->count=0;
}

void Program::process_step()
{
  if(this->state0 == 1)
  {
    Debug.println("entro-program");
    Debug.print(type[count][0]);
    inicio:
    switch (type[count][0]){
      case 'B':  //Begin
      letter = 'B';
      //this->count = count + 1;
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
