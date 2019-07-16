#include "comms.h"
#include <Arduino.h>

int const lenbuff=1024; // Longitud de buffer, Ajustar 128 512
                    // a lo que desees (o te sea posible)

unsigned int myaddress = 255;   //
char const beginchar = 0x02; //inicio
char const endchar = 0x04;   //fin
char const runchar = 0x33;   //run
char const pausechar = 0x34; //pause
char const stopchar = 0x35;  //stop
char const combchar = 0x43;  //C = I,V,T
char const ampchar = 0x49;   //I
char const voltchar = 0x56;  //V
char const tempchar = 0x54;  //T
char const writechar = 0x57; //W
char const readchar = 0x52;  //R
char const stepchar = 0x50;  //P
char const timechar = 0x74;  //t

char Ping = 0x64;  //default
bool flagload = false;

bool flagrun = false;
bool flagpause = false;

int toID;

void doTimeout(void);


int xbuff=0;      // Índice: siguiente char en cbuff
char cbuff[lenbuff]; // Buffer
//bool flagcommand=false;  // Flag para indicar comando disponible

void comms_inicbuff(void){ // Inicia a 0 cbuff -------------------
  int i;
  for(i=0;i<lenbuff;i++){   // Bucle que pone a 0 todos los
    cbuff[i]=0x00;          // caracteres en el buffer
  }
  xbuff=0;//0x00               // Inicializo el índice de siguiente
  flagbuff=true;                    // carácter
  Debug.println("limpio");
}

int comms_addcbuff(char c){ // Añade a cbuff -----------------------

  switch(c){

    case endchar:           // Enter -> Habilita Flag para procesar
      cbuff[xbuff]=c; // Añade carácter recibido al Buffer
      xbuff++;
      flagcommand=true;     // Comando en Main
      flagbuff=true;
      if(toID!=0)to.stop(toID);
      break;
    default:
     cbuff[xbuff]=c; // Añade carácter recibido al Buffer
     xbuff++;
     if(toID!=0)to.stop(toID);
     toID = to.after(100, doTimeout);
     break;
  }
}

void doTimeout(){
  to.stop(toID);
  if(!flagbuff){
    Debug.println("ERROR");
    comms_inicbuff();
  }
}

void comms_procesa_comando(void){

  char arg[lenbuff]; // Argumento de comando (si lo tiene)

  flagcommand=false; // Desactivo flag de comando pendiente.
  flagbuff =false;

  Debug.println("Procesando..."); // Monitorizo procesando ...
  /*for(int i=0;i<xbuff;i++){
    Debug.print(cbuff[i]);
  }
    Debug.print(xbuff);*/

  for(int i=0;i<lenbuff;i++){ // Bucle que pone a 0 todos los
    arg[i]=0x00; // caracteres en el argumento
  }

    if(cbuff[0]==beginchar&&cbuff[xbuff-1]==endchar){

      /*unsigned char tbuff[lenbuff]={0}; //1024

      int len=0;
      int n=3;
      if(cbuff[1]==Addfalse){n=2;}
      for(int x=n;; x++){
        if(cbuff[x]==0x03)break;

        tbuff[x-n]=cbuff[x]; // a partir del 3er byte y hasta 0.
        len++;
      }
      //Debug.print("len: ");
      //Debug.println(len);

      int dato = crc16_SingleBuf(tbuff,len);
      int crc16_high = highByte(dato);
      int crc16_low = lowByte(dato);

      char lowCrc = (char)crc16_low;
      char highCrc = (char)crc16_high;*/

      /*Debug.println(lowCrc);Debug.println(highCrc);
      Debug.println(cbuff[xbuff-3]);Debug.println(cbuff[xbuff-2]);*/

    /*  if(cbuff[xbuff-3]==lowCrc&&cbuff[xbuff-2]==highCrc) //validacion CRC
      {*/
          Debug.println("valido ");


            /*if(cbuff[1]==Addfalse&&cbuff[3]==0X03){ //write address
            Debug.println("writte-address");
            char add[] = {0};
            add[0]=cbuff[2];
            writeAddress(add);

            digitalWrite(LedComms, HIGH); Serial1.write(2); Serial1.print(Addfalse);Serial1.write("ACTION: PASS"); Serial1.write(3); Serial1.write(0); Serial1.write(0); Serial1.write(4);delay(2); digitalWrite(LedComms, LOW);
            myaddress = add[0];
            goto fin;
          }*/

          if(cbuff[1]==Ping&&cbuff[2]==myaddress){  //PING
            Debug.println("PING");
            digitalWrite(LedComms, HIGH); Serial1.write(2); Serial1.print(myaddress); Serial1.write("ACTION: PASS"); Serial1.write(3); Serial1.write(0);Serial1.write(0); Serial1.write(4);delay(2); digitalWrite(LedComms, LOW);

            flagcomms=true;
            goto fin;
          }

              if(cbuff[1]==myaddress) //address
              {
                  if(cbuff[2]==runchar){  //run
                    //clearProgram();
                    //loadProgram();
                    if(flagload){
                      flagrun = true; flagpause = false;
                      program.runStep();
                    }
                    else
                    {
                      digitalWrite(LedComms, HIGH); Serial1.write(2); Serial1.print(myaddress); Serial1.write(51); Serial1.write("ACTION: FAIL"); Serial1.write(3); Serial1.write(0);Serial1.write(0); Serial1.write(4);delay(2); digitalWrite(LedComms, LOW);
                    }
                  }

                  if(cbuff[2]==pausechar){  //pause
                    if(flagrun){
                      flagpause = true; flagrun=false;
                      program.pauseStep();
                    }
                    else
                    {
                      digitalWrite(LedComms, HIGH); Serial1.write(2); Serial1.print(myaddress); Serial1.write(52); Serial1.write("ACTION: FAIL"); Serial1.write(3); Serial1.write(0);Serial1.write(0); Serial1.write(4);delay(2); digitalWrite(LedComms, LOW);
                    }
                  }

                  if(cbuff[2]==stopchar){ //stop
                    if(flagpause||flagrun){
                      program.stopStep();
                    }
                    {
                      digitalWrite(LedComms, HIGH);Serial1.write(2);Serial1.print(myaddress); Serial1.write(53); Serial1.write("ACTION: FAIL"); Serial1.write(3); Serial1.write(0);Serial1.write(0); Serial1.write(4); delay(2); digitalWrite(LedComms, LOW);
                    }
                  }

                  if(cbuff[2]==combchar){  //show I,V,T

                    digitalWrite(LedComms, HIGH); Serial1.write(2);Serial1.print(myaddress);  Serial1.write("VALUE: ");Serial1.write("I");Serial1.print(valcurrent); Serial1.write(","); Serial1.write("V"); Serial1.print(valvoltage);Serial1.write(","); Serial1.write("T"); Serial1.print(valtemp); Serial1.write(3); Serial1.write(0); Serial1.write(0); Serial1.write(4);delay(2); digitalWrite(LedComms, LOW);
                  }

                  if(cbuff[2]==ampchar){  //show I
                    digitalWrite(LedComms, HIGH); Serial1.write(2);Serial1.print(myaddress); Serial1.write("I"); Serial1.write("VALUE: ");Serial1.print(valcurrent); Serial1.write(3); Serial1.write(0); Serial1.write(0); Serial1.write(4);delay(2); digitalWrite(LedComms, LOW);
                  }

                  if(cbuff[2]==voltchar){ //show V
                    digitalWrite(LedComms, HIGH); Serial1.write(2);Serial1.print(myaddress);Serial1.write("V");Serial1.write("VALUE: ");Serial1.print(valvoltage); Serial1.write(3); Serial1.write(0);Serial1.write(0); Serial1.write(4); delay(2); digitalWrite(LedComms, LOW);
                  }
                  if(cbuff[2]==tempchar){ //show T
                    digitalWrite(LedComms, HIGH); Serial1.write(2); Serial1.print(myaddress); Serial1.write("T");Serial1.write("VALUE: ");Serial1.print(valtemp); Serial1.write(3); Serial1.write(0);Serial1.write(0); Serial1.write(4);delay(2); digitalWrite(LedComms, LOW);
                  }

                  if(cbuff[2]==stepchar){ //current step
                    digitalWrite(LedComms, HIGH); Serial1.write(2);Serial1.print(myaddress);Serial1.write("P");Serial1.write("VALUE: ");Serial1.print(count+1);Serial1.print(letter);Serial1.write(3);Serial1.write(0);Serial1.write(0);Serial1.write(4);delay(2);digitalWrite(LedComms, LOW);
                  }

                  if(cbuff[2]==timechar){ //currentTime
                    digitalWrite(LedComms, HIGH);Serial1.write(2);Serial1.print(myaddress);Serial1.write("t");Serial1.write("VALUE: ");Serial1.print(controlTime.ms());Serial1.write(3);Serial1.write(0);Serial1.write(0);Serial1.write(4); delay(2);digitalWrite(LedComms, LOW);
                  }

                  if(cbuff[2]==writechar){  //writting eeprom Json
                      if(cbuff[3]==0x5B){
                        int i=3;
                        char tmp[lenbuff]={0};

                        do{ // Extraemos argumento del buffer
                          tmp[i-3]=cbuff[i]; // a partir del 3er byte y hasta 0.
                          //Debug.print(tmp[i-3]);
                          //Debug.println(i);
                        }while(cbuff[++i]!=0x03);
                        //Debug.print(tmp);
                        Debug.println("writingEEPROM");
                        //clearProgram();
                        eepromsave(tmp);
                        flagload = false;
                        digitalWrite(LedComms, HIGH); Serial1.write(2); Serial1.print(myaddress); Serial1.write(writechar); Serial1.write("ACTION: PASS"); Serial1.write(3); Serial1.write(0); Serial1.write(0); Serial1.write(4); delay(2); digitalWrite(LedComms, LOW);

                        clearProgram();
                        loadProgram();
                        Debug.println("lo cargo");
                      }
                      else
                      {
                        digitalWrite(LedComms, HIGH); Serial1.write(2); Serial1.print(myaddress); Serial1.write(writechar); Serial1.write("ACTION: FAIL"); Serial1.write(3);Serial1.write(0); Serial1.write(0); Serial1.write(4);delay(2);digitalWrite(LedComms, LOW);
                      }
                  }

                  if(cbuff[2]==readchar){ //reading eeprom - send Json
                    if(flagpause)
                    {
                      Debug.println("readingEEPROM");
                      clearProgram();
                      loadProgram();
                      for(int i=0; i <15;i++){
                        if(type[i][0] == 30){
                          break;
                        }
                        if(duration[i] == '\0'){
                            break;
                        }
                      }
                    }
                  }
              }
              else
              {
                Debug.println("invalido-Address");
              }
      /*}
      else
      {
        Debug.println("invalido");
      }*/

      fin:
      if(!flagbuff)comms_inicbuff(); // Borro buffer.
      Debug.println("Procesado"); // Monitorizo procesado.
    }
}
