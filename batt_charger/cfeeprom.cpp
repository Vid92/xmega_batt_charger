#include "cfeeprom.h"
#include <Arduino.h>

/*void writeAddress(char* add){
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
}*/
/*
char data_crc(String val){
  char buffer[1024] = {0};
  strcpy(buffer,val.c_str());

  int lenbff = 0;
  unsigned char tmp_crc[1024]={0};
  for(int i=0;i<val.length();i++)
  {
      tmp_crc[i]=buffer[i];
      lenbff++;
  }
  int dato = crc16_SingleBuf(tmp_crc,lenbff);
  outcrc16_high = highByte(dato);
  outcrc16_low = lowByte(dato);
}*/

void eepromsave(char* tmp){ //comparacion CR1 CR2 enviado
  x=0;
  for(int i = 0; i<1024;i++){ //256 512
    if(tmp[i]== 0xFF || tmp[i] == 0){
      break;
    }
    //EEPROM.write(i+1,tmp[i]);
    //i2c_eeprom.write(i+1,tmp[i]);
    x++;
    writeEEPROM(disk1,i+25,tmp[i]);
  }
  //Debug.print("x: ");
  //Debug.println(x);
  //Debug.print(tmp);
  //Debug.print("ok-eeprom");
}

char* IDread(){
  char idprog[25];
  memset(idprog,0,sizeof(idprog));

  for(int i=0;i<25;i++){
    int value= readEEPROM(disk1,i+1);
    if(value==0xFF || value == 0){
      break;
    }
    idprog[i] = (char)value;
  }

  digitalWrite(LedComms, HIGH); Serial1.write(2); Serial1.print(myaddress); Serial1.write("VALUE: "); Serial1.print("R"); Serial1.print(idprog); Serial1.write(3); Serial1.write(0); Serial1.write(0); Serial1.write(4);delay(2); digitalWrite(LedComms, LOW);
}

char* eepromread(){
  char temp[1024];
  memset(temp,0,sizeof(temp));
  Debug.println("temp");

    int read=0;
    for(int i = 0; i<1024;i++){
    //int val = EEPROM.read(i+1);//lee posicion
    //int val = i2c_eeprom.read(i+1);
    int val = readEEPROM(disk1,i+25);
    if(val== 0xFF || val == 0){
      break;
    }
    read++;
    temp[i] = (char)val;
  }
    Debug.print("read: ");
    Debug.println(read);
    Debug.print(temp);
    flagload = true;

    //digitalWrite(LedComms, HIGH); Serial1.write(2); Serial1.print(myaddress); Serial1.write("VALUE: "); Serial1.print("R"); Serial1.print(temp); Serial1.write(3); Serial1.write(0); Serial1.write(0); Serial1.write(4);delay(2); digitalWrite(LedComms, LOW);
    /*int val=0;
    for(int i=0;i<read;i++){
      Debug.print("a[");
      Debug.print(i);
      Debug.print("]=");
      Debug.print(temp[i],HEX);
      Debug.println();

      if(temp[i]==0x7B){
        for(i=i;i<read;i++){
          Debug.print(i);
          if(temp[i]==0x7D){
            Debug.println("step");
          }
        }
      }

      /*if(temp[i]==0x00){
        Serial.print("l=");
        Serial.println(i+1);
        break;
      }*/
    //}
    Debug.print("Json");

    StaticJsonBuffer<1024> jsonBuffer;
    JsonArray& root = jsonBuffer.parseArray(temp);

    //Debug.print(temp);

    if(!root.success())
    {
    	Debug.println("lectura() failed");
    }

    totalTime = 0;
    unsigned long totalDuration = 0;
    unsigned long totalAH = 0;
    unsigned long timeAH = 0;

    for(int i= 0; i<17; i++){
      const char* type0 = root[i]["Type"];
      if(type[0] == '\0'){
        break;
      }
      unsigned long time = root[i]["Time"];
      float currentAH = root[i]["AH"];

      float current0 = root[i]["Current"];
      float temperature1 = root[i]["MaxTemp"];
      float temperature2 = root[i]["MinTemp"];

      strcpy(type[i],type0);
      duration[i]=time;
      totalDuration = totalDuration + duration[i];
      //Debug.println(totalDuration);

      current[i]=current0;
      AmperH[i]=currentAH;
      timeAH = AmperH[i] / (0.000277 * current[i]);
      //Debug.println(timeAH);
      totalAH = totalAH + timeAH;
      //Debug.println(totalAH);
      maxtemp[i]=temperature1;
      mintemp[i]=temperature2;
    }
    totalTime = totalAH + totalDuration;
    Debug.println(totalTime);
}

void clearProgram(){
  for(int i=0; i< 17;i++){
    memset(&type[i],0,20*sizeof(char));
  }
  for(int i=0; i< 17;i++){
    duration[i]=0;
    AmperH[i]=0;
    current[i]=0;
    maxtemp[i]=0;
    mintemp[i]=0;
  }
  Debug.println("clean");
}

void loadProgram(){
  eepromread();
}
