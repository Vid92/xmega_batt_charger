#include "comms.h"
#include <Arduino.h>

int const lenbuff=1024; // Longitud de buffer, Ajustar 128 512 1024
                    // a lo que desees (o te sea posible)
unsigned int myaddress = 255;   //
const char beginchar = 0x02; //inicio
const char endchar = 0x04;   //fin
const char runchar = 0x33;   //run
const char pausechar = 0x34; //pause
const char stopchar = 0x35;  //stop
const char combchar = 0x43;  //C = I,V,T
const char ampchar = 0x49;   //I
const char voltchar = 0x56;  //V
const char tempchar = 0x54;  //T
const char writechar = 0x57; //W
const char readchar = 0x52;  //R
const char stepchar = 0x50;  //P
const char timechar = 0x74;  //t

const char Ping = 0x64;  //default
bool flagload = false;

bool flagrun = false;
bool flagpause = false;
float totalTime;
String totalhms; //total time formato "00:00:00"

//unsigned long Ttime = 0; //variable acum.tiempo
float TempTime1 = 0; // aux AH acumulado
//float TempTime2 = 0; //unsigned long
float totAH = 0.0;   //variable acum.AH

int toID;
int save = 0;
int read = 0;

int xbuff=0;      // Índice: siguiente char en cbuff
char cbuff[lenbuff]; // Buffer
//bool flagcommand=false;  // Flag para indicar comando disponible

//-------------------------------- Funciones ----------------------------------//
void doTimeout(void);
void aux_crc(void);


//----------------------------- clean buffer ----------------------------------//
void comms_inicbuff(void){ // Inicia a 0 cbuff
  int i;
  for(i=0;i<lenbuff;i++){   // Bucle que pone a 0 todos los
    cbuff[i]=0x00;          // caracteres en el buffer
  }
  xbuff=0;//0x00            // Inicializo el índice de siguiente
  flagbuff=true;                    // carácter
  //Debug.println("limpio");
}

//--------------------------- add data to buffer -----------------------------//
int comms_addcbuff(char c){ // Añade a cbuff
  switch(c){
    case endchar:           // Enter -> Habilita Flag para procesar
      cbuff[xbuff]=c;       // Añade carácter recibido al Buffer
      xbuff++;
      flagcommand=true;     // Comando en Main
      flagbuff=true;
      if(toID!=0)to.stop(toID);
      break;
    default:
     cbuff[xbuff]=c;       // Añade carácter recibido al Buffer
     xbuff++;
     if(toID!=0)to.stop(toID);
     toID = to.after(100, doTimeout); // 115200 - 100  9600- 1000
     break;
  }
}

//--------------------------------- DoTimeOut --------------------------------//
void doTimeout(){
  to.stop(toID);
  if(!flagbuff){
    //Debug.println("ERROR");
    comms_inicbuff();
  }
}

//---------------------------- Procesa comando -------------------------------//
void comms_procesa_comando(void){

  char arg[lenbuff];  // Argumento de comando (si lo tiene)
  flagcommand=false;  // Desactivo flag de comando pendiente.
  flagbuff =false;

  for(int i=0;i<lenbuff;i++){ // Bucle que pone a 0 todos los
    arg[i]=0x00;      //caracteres en el argumento
  }
    int len =0;
    if(cbuff[0]==beginchar&&cbuff[xbuff-4]==0x03&&cbuff[xbuff-1]==endchar){
     unsigned char tbuff[lenbuff]={0}; //1024
      char crc16_high;
      char crc16_low;
      int n=3;
      for(int x=n;; x++){
        if(cbuff[x]==0x03)break;

        tbuff[x-n]=cbuff[x]; // a partir del 3er byte y hasta 0.
        len++;
      }

      if(len<lenbuff){
        int dato = crc16_SingleBuf(tbuff,len);
        crc16_high = highByte(dato);
        crc16_low = lowByte(dato);
      }

      //----------------------------- validacion CRC ------------------------------//
      if(cbuff[xbuff-3]==crc16_low && cbuff[xbuff-2]==crc16_high) //validacion CRC
      {
        Debug.println("valido crc");
        if(cbuff[1]==myaddress) //address
        {
            //Debug.println("valido address");
            if(cbuff[2]==Ping){
              Debug.println("PING");

              String val = String(myaddress)+"ACTION: PASS";
              //int dato0 = aux_crc(val);
              char bffer[1024] = {0};
              strcpy(bffer,val.c_str());

              int lenbff = 0;
              unsigned char tmp_crc[1024]={0};
              for(int i=0;i<val.length();i++)
              {
                  tmp_crc[i]=bffer[i];
                  lenbff++;
              }
              int dato = crc16_SingleBuf(tmp_crc,lenbff);
              crc16_high = highByte(dato);
              crc16_low = lowByte(dato);

              digitalWrite(LedComms, HIGH); Serial1.write(2); Serial1.print(myaddress); Serial1.write("ACTION: PASS"); Serial1.write(3); Serial1.write(crc16_low);Serial1.write(crc16_high); Serial1.write(4);delay(2); digitalWrite(LedComms, LOW);
            }

            if(cbuff[2]==runchar){  //run
                if(!flagload){Debug.println("cargo");clearProgram();loadProgram();}
                if(flagload){
                  flagrun = true; flagpause = false;
                  program.runStep();
                }
                else
                {
                  digitalWrite(LedComms, HIGH); Serial1.write(2); Serial1.print(myaddress); Serial1.write(51); Serial1.write("ACTION: FAIL,RUN"); Serial1.write(3); Serial1.write(0);Serial1.write(0); Serial1.write(4); delay(2); digitalWrite(LedComms, LOW);
                }
            }

            if(cbuff[2]==pausechar){  //pause
                if(flagrun){
                  flagpause = true; flagrun=false;
                  program.pauseStep();
                }
                else
                {
                  digitalWrite(LedComms, HIGH); Serial1.write(2); Serial1.print(myaddress); Serial1.write(52); Serial1.write("ACTION: FAIL,PAUSE"); Serial1.write(3); Serial1.write(0);Serial1.write(0); Serial1.write(4);delay(2); digitalWrite(LedComms, LOW);
                }
            }

            if(cbuff[2]==stopchar){ //stop
                if(flagpause||flagrun){
                  program.stopStep();
                }
                if(!flagpause&&!flagrun)
                {
                  digitalWrite(LedComms, HIGH);Serial1.write(2);Serial1.print(myaddress); Serial1.write(53); Serial1.write("ACTION: FAIL,STOP"); Serial1.write(3); Serial1.write(0); Serial1.write(0); Serial1.write(4); delay(2); digitalWrite(LedComms, LOW);
                }
            }

            if(cbuff[2]==combchar){  //show I,V,T
                //TempTime2 = Ttime + (controlTime.ms()*0.001);
                TempTime1 = totAH + valAH;

                String val = String(myaddress) + "VALUE: " "I"+ String(valcurrent)+"," "V" + String(valvoltage)+"," "T" +String(valtemp)+"," "AH"+String(valAH)+"," "AC"+String(TempTime1)+"," "P"+(count)+"," "S"+letter+"," "t"+timehms+"," "Tt"+timehmsa+"," "TT" +totalhms+"," "N"+nameProg+","+stepState;

                char bffer[1024] = {0};
                strcpy(bffer,val.c_str());

                int lenbff = 0;
                unsigned char tmp_crc[1024]={0};
                for(int i=0;i<val.length();i++)
                {
                    tmp_crc[i]=bffer[i];
                    lenbff++;
                }
                int dato = crc16_SingleBuf(tmp_crc,lenbff);
                //int dato0 = aux_crc(val);
                crc16_high = highByte(dato);
                crc16_low = lowByte(dato);

                digitalWrite(LedComms, HIGH); Serial1.write(2);Serial1.print(myaddress);  Serial1.write("VALUE: ");Serial1.write("I");Serial1.print(valcurrent); Serial1.write(","); Serial1.write("V"); Serial1.print(valvoltage);Serial1.write(","); Serial1.write("T"); Serial1.print(valtemp);Serial1.write(",");Serial1.write("AH");Serial1.print(valAH);Serial1.write(",");Serial1.write("AC");Serial1.print(TempTime1);Serial1.write(",");Serial1.write("P"); Serial1.print(count);Serial1.write(",");Serial1.write("S");Serial1.write(letter);Serial1.write(","); Serial1.write("t"); Serial1.print(timehms);Serial1.write(",");Serial1.write("Tt"); Serial1.print(timehmsa); Serial1.write(","); Serial1.write("TT"); Serial1.print(totalhms); Serial1.write(",");Serial1.write("N"); Serial1.print(nameProg); Serial1.write(","); Serial1.print(stepState);Serial1.write(3); Serial1.write(crc16_low); Serial1.write(crc16_high); Serial1.write(4);delay(2); digitalWrite(LedComms, LOW);
            }

            if(cbuff[2]==writechar){  //writting eeprom Json
              if(cbuff[8]==0x5B){ //3  OJO!! 28 0x6B = [
                  int i=3;
                  char tmp[lenbuff]={0};
                  do{ // Extraemos argumento del buffer
                    tmp[i-3]=cbuff[i]; // a partir del 3er byte y hasta 0.
                  }while(cbuff[++i]!=0x03);
                  Debug.println("writingEEPROM");

                  //cleanEeprom(); //--*
                  eepromsave(tmp);
                  flagload = false;
                  //Debug.println(len);
                  //Debug.println(save);
                  if(len==save)
                  {
                    clearProgram();
                    jsonSave(tmp);
                    //jsonOrigenSave(tmp);
                    //if (save == read){
                    if(flagload){
                      digitalWrite(LedComms, HIGH); Serial1.write(2); Serial1.print(myaddress); Serial1.write(writechar); Serial1.write("ACTION: PASS"); Serial1.write(3); Serial1.write(0); Serial1.write(0); Serial1.write(4); delay(2); digitalWrite(LedComms, LOW);
                    }
                    else{
                      digitalWrite(LedComms, HIGH); Serial1.write(2); Serial1.print(myaddress); Serial1.write(writechar); Serial1.write("ACTION: FAIL"); Serial1.write(3);Serial1.write(0); Serial1.write(0); Serial1.write(4);delay(2); digitalWrite(LedComms, LOW);
                    }
                  }
              }
            }

            if(cbuff[2]==readchar){ //reading eeprom -IDProgram
                Debug.println("readingIDProgram");
                clearProgram();
                IDread();
                //eepromread();
            }
          }
          //else
          //{
          //  Debug.println("invalido-Address");
          //}
      }
      //else
      //{
      //  Debug.println("invalido crc");
      //}
      if(!flagbuff)comms_inicbuff(); // Borro buffer.
      //Debug.println("Procesado"); // Monitorizo procesado.
    }
}
