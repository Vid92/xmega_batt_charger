#include "cfeeprom.h"
#include <Arduino.h>

void writeAddress(char* add){
  //i2c_eeprom.write(0,add[0]);
  writeEEPROM(disk1,0,add[0]);
  //EEPROM.write(0,add[0]);
}

char* readAddress(){
    char addR[0];
    memset(addR,0,sizeof(addR));

    //int value = i2c_eeprom.read(0);
    int value = readEEPROM(disk1,0);
    //int val = EEPROM.read(0);
    //myaddress = value;
}

void eepromsave(char* tmp){ //comparacion CR1 CR2 enviado
  for(int i = 0; i<1024;i++){ //256 512
    /*if(tmp[i]== 0xFF || tmp[i] == 0){
      break;
    }*/
    //EEPROM.write(i+1,tmp[i]);
    //i2c_eeprom.write(i+1,tmp[i]);
    writeEEPROM(disk1,i+1,tmp[i]);
  }
  Debug.print(tmp);
  //Debug.print("ok-eeprom");
}

char* eepromread(){
  char temp[1024];
  //unsigned char temp[1024];
  memset(temp,0,sizeof(temp));

    //int len=0;
    int val=0;
    for(int i = 0; i<1024;i++){
    //int val = EEPROM.read(i+1);//lee posicion
    //int val = i2c_eeprom.read(i+1);
    val = readEEPROM(disk1,i+1);
    if(val== 0xFF || val == 0){
      break;
    }
    //len++;
    temp[i] = (char)val;
  }
    Debug.print(temp);

    flagload = true;

    if(flagpause){digitalWrite(LedComms, HIGH); Serial1.write(2); Serial1.print(myaddress); Serial1.write("VALUE: "); Serial1.print("R"); Serial1.print(temp); Serial1.write(3); Serial1.write(0); Serial1.write(0); Serial1.write(4);delay(2); digitalWrite(LedComms, LOW);}

    /*for(int i=0;i<len;i++){
      Debug.print("a[");
      Debug.print(i);
      Debug.print("]=");
      Debug.print(temp[i],HEX);
      Debug.println();

      /*if(temp[i]==0x00){
        Serial.print("l=");
        Serial.println(i+1);
        break;
      }
    }*/
    Debug.print(sizeof(temp));

    StaticJsonBuffer<512> jsonBuffer;//2048
    JsonArray& root = jsonBuffer.parseArray(temp);

    Debug.print(temp);

    /*if (!root.success())
    {
    	Debug.println("lectura() failed");
    }*/
    totalTime = 0;
    for(int i= 0; i<15; i++){
      const char* type0 = root[i]["Type"];
      if(type[0] == '\0'){
        break;
      }
      unsigned long time = root[i]["Time"];
      float current0 = root[i]["Current"];
      float temperature1 = root[i]["Maxtemp"];
      float temperature2 = root[i]["Mintemp"];


      strcpy(type[i],type0);
      duration[i]=time;
      totalTime = totalTime + duration[i];
      //Debug.println(totalTime);

      current[i]=current0;
      maxtemp[i]=temperature1;
      mintemp[i]=temperature2;
    }
    //Debug.print("Total:");
    Debug.println(totalTime);
}

void clearProgram(){
  for(int i=0; i< 15;i++){
    memset(&type[i],0,20*sizeof(char));
  }
  for(int i=0; i< 15;i++){
    duration[i]=0;
    current[i]=0;
    maxtemp[i]=0;
    mintemp[i]=0;
  }
  Debug.println("clean");
}

void loadProgram(){
  eepromread();
}
