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
  //unsigned char tmp2[1024];
  //int len = 0;
  for(int i = 0; i<1024;i++){ //256 512
    /*if(tmp[i]== 0xFF || tmp[i] == 0){
      break;
    }*/
    //EEPROM.write(i+1,tmp[i]);
    //i2c_eeprom.write(i+1,tmp[i]);
    writeEEPROM(disk1,i+1,tmp[i]);
    //len++;
  }
  //tmp2[len]= tmp[len];
  //int crc16 = crc16_SingleBuf(tmp2,len);
  //Debug.print(tmp);
}

char* eepromread(){
  char temp[1024];
  memset(temp,0,sizeof(temp));

  //unsigned char temp2[1024];
  int len = 0;
  for(int i = 0; i<1024;i++){
    //int val = EEPROM.read(i+1);//lee posicion
    //int val = i2c_eeprom.read(i+1);
    int val = readEEPROM(disk1,i+1);
    if(val== 0xFF || val == 0){
      break;
    }
    temp[i] = (char)val;
    len++;
    //Debug.print(temp);
  }
    //Debug.print(temp);

    flagload = true;

    //Debug.print(len);
    //temp2[len]= temp[len];

    //int crc16 = crc16_SingleBuf(temp2,len);
    //int crc16_high = highByte(crc16);
    //int crc16_low = lowByte(crc16);

    //Debug.print(crc16_low); Debug.print(crc16_high);

    if(flagpause){digitalWrite(LedComms, HIGH); Serial1.write(2); Serial1.print(myaddress); Serial1.write("VALUE: "); Serial1.print("R"); Serial1.print(temp); Serial1.write(3); Serial1.write(0); Serial1.write(0); Serial1.write(4);delay(2); digitalWrite(LedComms, LOW);}

    /*for(int i=0;i<512;i++){
      Serial.print("a[");
      Serial.print(i);
      Serial.print("]=");
      Serial.print(temp[i],HEX);
      Serial.println();

      /*if(temp[i]==0x00){
        Serial.print("l=");
        Serial.println(i+1);
        break;
      }
    }*/

    StaticJsonBuffer<2048> jsonBuffer;
    JsonArray& root = jsonBuffer.parseArray(temp);

    //Serial1.print(temp);

    /*if (!root.success())
    {
    	Debug.println("lectura() failed");
    }*/

    for(int i= 0; i<15; i++){
      const char* type0 = root[i]["Type"];
      if(type[0] == '\0'){
        break;
      }
      unsigned long time = root[i]["Time"];
      float current0 = root[i]["Current"];

      strcpy(type[i],type0);
      duration[i]=time;
      current[i]=current0;
    }
}

void clearProgram(){
  for(int i=0; i< 15;i++){
    memset(&type[i],0,20*sizeof(char));
  }
  for(int i=0; i< 15;i++){
    duration[i]=0;
    current[i]=0;
  }
}

void loadProgram(){
  eepromread();
}
