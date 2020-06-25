#include "comms.h"
#include <Arduino.h>

int const lenbuff=1024; // Longitud de buffer, Ajustar 128 512
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
unsigned long totalTime;
unsigned long Ttime=0;
float TempTime1 = 0;
float TempTime2 = 0;


int toID;
int x = 0;

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
  //Debug.println("limpio");
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
     toID = to.after(1000, doTimeout); // 115200 - 100  9600- 1000
     break;
  }
}

void doTimeout(){
  to.stop(toID);
  if(!flagbuff){
    //Debug.println("ERROR");
    comms_inicbuff();
  }
}

void comms_procesa_comando(void){

  char arg[lenbuff]; // Argumento de comando (si lo tiene)

  flagcommand=false; // Desactivo flag de comando pendiente.
  flagbuff =false;

  //Debug.println("Procesando..."); // Monitorizo procesando ...
  /*for(int i=0;i<xbuff;i++){
    Debug.print(cbuff[i]);
  }
    Debug.print(xbuff);*/

  for(int i=0;i<lenbuff;i++){ // Bucle que pone a 0 todos los
    arg[i]=0x00; // caracteres en el argumento
  }
    int len =0;
    //Debug.println(cbuff[0]);
    //Debug.println(cbuff[xbuff-4]);
    //Debug.println(cbuff[xbuff-1]);
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

        //Debug.print("entroLEN");
        int dato = crc16_SingleBuf(tbuff,len);
        crc16_high = highByte(dato);
        crc16_low = lowByte(dato);
      }

      if(cbuff[xbuff-3]==crc16_low && cbuff[xbuff-2]==crc16_high) //validacion CRC
      {
        Debug.println("valido crc");
        if(cbuff[1]==myaddress) //address
        {
            //Debug.println("valido address");
            if(cbuff[2]==Ping){
              Debug.println("PING");

              String val = String(myaddress)+"ACTION: PASS";
              //data_crc(val);
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
                  digitalWrite(LedComms, HIGH); Serial1.write(2); Serial1.print(myaddress); Serial1.write(51); Serial1.write("ACTION: FAIL"); Serial1.write(3); Serial1.write(0);Serial1.write(0); Serial1.write(4); delay(2); digitalWrite(LedComms, LOW);
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
                if(!flagpause&&!flagrun)
                {
                  digitalWrite(LedComms, HIGH);Serial1.write(2);Serial1.print(myaddress); Serial1.write(53); Serial1.write("ACTION: FAIL"); Serial1.write(3); Serial1.write(0);Serial1.write(0); Serial1.write(4); delay(2); digitalWrite(LedComms, LOW);
                }
            }

            if(cbuff[2]==combchar){  //show I,V,T

                TempTime1 = controlTime.ms()*0.001;
                TempTime2 = Ttime + (controlTime.ms()*0.001);

                String val = String(myaddress) + "VALUE: " "I"+ String(valcurrent)+"," "V" + String(valvoltage)+"," "T" +String(valtemp)+ "," "P"+(count+1)+letter+"," "t"+TempTime1+"," "Tt"+TempTime2+"," "TT" + totalTime+","+ stepState;
                //data_crc(val);

                //Debug.println(val);
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

                digitalWrite(LedComms, HIGH); Serial1.write(2);Serial1.print(myaddress);  Serial1.write("VALUE: ");Serial1.write("I");Serial1.print(valcurrent); Serial1.write(","); Serial1.write("V"); Serial1.print(valvoltage);Serial1.write(","); Serial1.write("T"); Serial1.print(valtemp);Serial1.write(",");Serial1.write("P"); Serial1.print(count+1);Serial1.print(letter); Serial1.write(","); Serial1.write("t"); Serial1.print(TempTime1);Serial1.write(",");Serial1.write("Tt"); Serial1.print(TempTime2); Serial1.write(","); Serial1.write("TT"); Serial1.print(totalTime); Serial1.write(","); Serial1.print(stepState); Serial1.write(3); Serial1.write(crc16_low); Serial1.write(crc16_high); Serial1.write(4);delay(2); digitalWrite(LedComms, LOW);
            }

            if(cbuff[2]==writechar){  //writting eeprom Json

              if(cbuff[3]==0x5B){ //OJO!! 28
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
                  for (int i = 1 ; i <1024 ; i++) { //27
                    writeEEPROM(disk1,i,0);
                  }
                  eepromsave(tmp);
                  flagload = false;

                  if(len==x)
                  {
                    String val = String(myaddress) + String(writechar)+ "ACTION: PASS";
                    //data_crc(val);
                    digitalWrite(LedComms, HIGH); Serial1.write(2); Serial1.print(myaddress); Serial1.write(writechar); Serial1.write("ACTION: PASS"); Serial1.write(3); Serial1.write(crc16_low); Serial1.write(crc16_high); Serial1.write(4); delay(2); digitalWrite(LedComms, LOW);

                    uint8_t oldInterruptState = SREG;  // no real need to store the interrupt context as the reset will pre-empt its restoration
                    asm("cli");                        // Disable interrupts

                    CCP = 0xD8;                        // Configuration change protection: allow protected IO regiser write
                    RST.CTRL = RST_SWRST_bm;           // Request software reset by writing to protected IO register

                    SREG=oldInterruptState;           // Restore interrupts enabled/disabled state (out of common decency - this line will never be reached because the reset will pre-empt it)
                  }

                  else
                  {
                    String val = String(myaddress)+String(writechar)+"ACTION: FAIL";
                    /*
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
                    outcrc16_high = highByte(dato);
                    outcrc16_low = lowByte(dato);*/

                    //data_crc(val);

                    digitalWrite(LedComms, HIGH); Serial1.write(2); Serial1.print(myaddress); Serial1.write(writechar); Serial1.write("ACTION: FAIL"); Serial1.write(3);Serial1.write(crc16_low); Serial1.write(crc16_high); Serial1.write(4);delay(2); digitalWrite(LedComms, LOW);
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
          else
          {
            Debug.println("invalido-Address");
          }
          
      }
      else
      {
        Debug.println("invalido crc");
      }

      //fin:
      if(!flagbuff)comms_inicbuff(); // Borro buffer.
      //Debug.println("Procesado"); // Monitorizo procesado.
    }
}
