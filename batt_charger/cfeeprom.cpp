#include "cfeeprom.h"
#include <Arduino.h>

void eepromsave(char* tmp){ //comparacion CR1 CR2 enviado
  save=0;
  for(int i = 0; i<1024;i++){ //13400 por todos los prog.
    if(tmp[i]== 0xFF || tmp[i] == 0){
      break;
    }
    //EEPROM.write(i+1,tmp[i]);
    //i2c_eeprom.write(i+1,tmp[i]);
    save++;
    writeEEPROM(disk1,i+10,tmp[i]); //0-10 prog.default
  }
}

void jsonOrigenSave(char* temp){
  Debug.println(temp);
  //DynamicJsonBuffer jsonBuffer(100);
  StaticJsonBuffer<2500> jsonBuffer; //tamaño a cambiar
  JsonArray& root = jsonBuffer.parseArray(temp);

  if(!root.success())
  {
    Debug.println("lectura failed");
  }
  else{
    flagload = true;
    totalTime = 0;
    float totalDuration = 0;
    float totalAH = 0;
    float timeAH = 0;

    for(int i= 0; i<17; i++){
      const char* type0 = root[i]["Type"];
      float time = root[i]["Time"];
      float currentAH = root[i]["AH"];

      float current0 = root[i]["Current"];
      float temperature1 = root[i]["MaxTemp"];
      float temperature2 = root[i]["MinTemp"];

      if(type0){
        strlcpy(type[i], type0,sizeof(type[i]));
      }
      else{
        break;
      }

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
}

void jsonSave(char* temp){

  Debug.println(temp);

  int count = 0;
  const char* type0;
  float time;
  float current0;
  float currentAH;
  float temperature1;
  float temperature2;
  totalTime = 0.0;
  float totalDuration = 0.0;
  float timeAH = 0.0;
  float totalAH = 0.0;
  for(int i=0;i<strlen(temp);i++){
    if(temp[i] == 'T' && temp[i+1] == 'y' && temp[i+2] == 'p' && temp[i+3] == 'e'){
      count +=1;
      type0 = "Type";
  		if(temp[i+7] == 'B')
        type0 = "Begin";
  		else if(temp[i+7] == 'P')
        type0 = "Pause";
  		else if(temp[i+7] == 'C')
        type0 = "Charge";
  		else if(temp[i+7] == 'E')
        type0 = "End";
    }
  	else if(temp[i] == 'T' && temp[i+1] == 'i' && temp[i+2] == 'm' && temp[i+3] == 'e'){
      if(temp[i+9] == ',' || temp[i+9] == '}'){
        String x = String(temp[i+7]);
        time = x.toFloat();}
      else if(temp[i+10] == ',' || temp[i+10] == '}'){
        String x = String(temp[i+7])+String(temp[i+8]);
        time = x.toFloat();}
      else if(temp[i+11] == ',' || temp[i+11] == '}'){
        String x = String(temp[i+7])+String(temp[i+8])+String(temp[i+9]);
        time = x.toFloat();}
      else if(temp[i+12] == ',' || temp[i+12] == '}'){
        String x = String(temp[i+7])+String(temp[i+8])+String(temp[i+9])+String(temp[i+10]);
        time = x.toFloat();}
      else if(temp[i+13] == ',' || temp[i+13] == '}'){
        String x = String(temp[i+7])+String(temp[i+8])+String(temp[i+9])+String(temp[i+10])+String(temp[i+11]);
        time = x.toFloat();}
      else if(temp[i+14] == ',' || temp[i+14] == '}'){
        String x = String(temp[i+7])+String(temp[i+8])+String(temp[i+9])+String(temp[i+10])+String(temp[i+11])+String(temp[i+12]);
        time = x.toFloat();
      }
      duration[count-1]=time * 3600.0;
      totalDuration = totalDuration + duration[count-1];
    }
  	else if(temp[i] == 'C' && temp[i+1] == 'u' && temp[i+2] == 'r' && temp[i+3] == 'r' && temp[i+4] == 'e' && temp[i+5] == 'n' && temp[i+6] == 't'){
      if(temp[i+12] == ','){
        String x = String(temp[i+10]);
        current0 = x.toFloat();
      }
      else if(temp[i+13] == ','){
        String x = String(temp[i+10])+String(temp[i+11]);
        current0 = x.toFloat();
      }
      else if(temp[i+14] == ','){
        String x = String(temp[i+10])+String(temp[i+11])+String(temp[i+12]);
        current0 = x.toFloat();
      }
      else if(temp[i+15] == ','){
        String x = String(temp[i+10])+String(temp[i+11])+String(temp[i+12])+String(temp[i+13]);
        current0 = x.toFloat();
      }
      current[count-1]=current0;
    }
    else if(temp[i] == 'A' && temp[i+1] == 'H'){
      if(temp[i+7] == ','){
        String x = String(temp[i+5]);
        currentAH = x.toFloat();}
      else if(temp[i+8] == ','){
        String x = String(temp[i+5])+String(temp[i+6]);
        currentAH = x.toFloat();}
      else if(temp[i+9] == ','){
        String x = String(temp[i+5])+String(temp[i+6])+String(temp[i+7]);
        currentAH = x.toFloat();}
      else if(temp[i+10] == ','){
        String x = String(temp[i+5])+String(temp[i+6])+String(temp[i+7])+String(temp[i+8]);
        currentAH = x.toFloat();}
      else if(temp[i+11] == ','){
        String x = String(temp[i+5])+String(temp[i+6])+String(temp[i+7])+String(temp[i+8])+String(temp[i+9]);
        currentAH = x.toFloat();}
      else if(temp[i+12] == ','){
        String x = String(temp[i+5])+String(temp[i+6])+String(temp[i+7])+String(temp[i+8])+String(temp[i+9])+String(temp[i+10]);
        currentAH = x.toFloat();}
      AmperH[count-1]=currentAH;
      timeAH = AmperH[count-1] / (0.000277 * current[count-1]);
      totalAH = totalAH + timeAH;
      }
    else if(temp[i] == 'M' && temp[i+1] == 'a' && temp[i+2] == 'x' && temp[i+3] == 'T' && temp[i+4] == 'e' && temp[i+5] == 'm' && temp[i+6] == 'p'){
      if(temp[i+12] == ','){
        String x = String(temp[i+10]);
        temperature1 = x.toFloat();}
      else if(temp[i+13] == ','){
        String x = String(temp[i+10])+String(temp[i+11]);
        temperature1 = x.toFloat();
      }
      else if(temp[i+14] == ','){
        String x = String(temp[i+10])+String(temp[i+11])+String(temp[i+12]);
        temperature1 = x.toFloat();
      }
      else if(temp[i+15] == ','){
        String x = String(temp[i+10])+String(temp[i+11])+String(temp[i+12])+String(temp[i+13]);
        temperature1 = x.toFloat();
      }
      maxtemp[count-1]=temperature1;
    }

    else if(temp[i] == 'M' && temp[i+1] == 'i' && temp[i+2] == 'n' && temp[i+3] == 'T' && temp[i+4] == 'e' && temp[i+5] == 'm' && temp[i+6] == 'p'){
      if(temp[i+12] == '}'){
        String x = String(temp[i+10]);
        temperature2 = x.toFloat();}
      else if(temp[i+13] == '}'){
        String x = String(temp[i+10])+String(temp[i+11]);
        temperature2 = x.toFloat();
      }
      else if(temp[i+14] == '}'){
        String x = String(temp[i+10])+String(temp[i+11])+String(temp[i+12]);
        temperature2 = x.toFloat();
      }
      else if(temp[i+15] == '}'){
        String x = String(temp[i+10])+String(temp[i+11])+String(temp[i+12])+String(temp[i+13]);
        temperature2 = x.toFloat();
      }
      mintemp[count-1]=temperature2;
    }
    if(type0){
      strlcpy(type[count-1], type0,sizeof(type[count-1]));
    }
    else{
      break;
    }
  }
  totalTime = totalAH + totalDuration;
  Debug.println(totalTime);
  flagload = true;
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

void eepromread(){
  char eeprom[1024];
  memset(eeprom,0,sizeof(eeprom));

  read=0;
  for(int i = 0; i<1024;i++){
    int val = readEEPROM(disk1,i+10);
    if(val==0xFF||val== 0){
      break;
    }
    read++;
    eeprom[i] = char(val);
  }
    //Debug.print("read: ");
    //Debug.println(read);

    jsonSave(eeprom);
}

void clearProgram(){
  for(int i=0; i< 17;i++){
    memset(&type[i],0,10*sizeof(char));
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

void cleanEeprom(){
  for (int i = 0; i <1024 ; ++i) { //1024 1330
    writeEEPROM(disk1,i,0);
  }
}

void loadProgram(){
  eepromread();
}
