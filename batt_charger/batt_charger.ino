#include "comms.h"
#include "xmDAC.h"
#include "Timer.h"
#include "globals.h"

int disk1 = 0x50;    //Address of 24LC256 eeprom chip
int disk2 = 0x57;    //Address of 24LC256 eeprom chip extern

unsigned int address = 0;

char rcvchar=0x00;   // último carácter recibido
bool flagcommand=false;
bool flagbuff = false;
bool flagStep=false;
bool flagtime=true;
int count = 0;
String letter = "I"; //0x49; //I char
char stepState = 0x49;

String timehms="00:00:00";

int seg = 0;
int temSeg = 0;
int mint = 0;
int temMin = 0;
int hor = 0;

//String timehms = ""; //agregar dias !
String mint0 = "";
String hor0 = "";
String seg0 = "";

//int i=0;
//int state0 = 0;
//int prevstate0 = 0;

double valcurrent = 0; //solo para mostrar
double valvoltage = 0;
double valtemp = 0;
float valAH = 0;

StopWatch controlTime;
Control control;
Program program;
//I2CEEPROM i2c_eeprom(0x50);

Timer t;
Timer to;
int LedComms = 17;
int LedRelay = 20;

char anbu[1024];  //cfg json
char type[17][20]; //inicio, pausa,carga, fin
unsigned long duration[17]; //time
float AmperH[17];
float current[17];
float maxtemp[17];
float mintemp[17];

void setup()
{
  //baudios
  Debug.begin(115200);
  Serial1.begin(115200);
  Wire.begin();
  control.begin();

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LedComms, OUTPUT);
  pinMode(LedRelay, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(LedComms, LOW);
  digitalWrite(LedRelay, LOW);

  myaddress = readEEPROM(disk2, address);

  writeEEPROM(disk1, address,myaddress);

  clearProgram();
  loadProgram();

  t.every(1000,printMessage); //mostrar valores
}

void loop()
{
  if(flagcommand)comms_procesa_comando();

  t.update();
  to.update();

  control.readData();
  program.process_step();
  control.event();


  if(flagStep)
  {
    flagStep = false;
    program.nextStep();
  }
}

void printMessage()
{
    temSeg = controlTime.ms()*0.001;
    if(temSeg > 3599){
      hor = temSeg / 3600; // horas
      temMin = temSeg - (hor * 3600);
      hor0 = String(hor)+":";
      if (temMin > 59){
        mint = temMin / 60; //minutos
        seg = temMin - (mint * 60); //segundos
        if(seg<10){
          seg0 = "0"+String(seg);
        }
        else{
          seg0 = String(seg);
        }
        if(mint<10){
          mint0 = "0"+String(mint)+":";
        }
        else{
          mint0 = String(mint)+":";
        }
      }
      else{
        seg = temMin;
        if(seg<10){
          seg0 = "0"+String(seg);
        }
        else{
          seg0 = String(seg);
        }
      }
    }
    else if (temSeg > 59){
      mint = temSeg / 60; //minutos
      seg = temSeg - (mint * 60); //segundos

      if (seg<10){
        seg0 = "0"+String(seg);
      }
      else{
        seg0 = String(seg);
      }
      if(mint<10){
        mint0 = "0"+String(mint)+":";
      }
      else{
        mint0 = String(mint)+":";
      }
      hor0 = "00:";
    }
    else if(temSeg < 59){
      seg = temSeg;
      if (seg < 10){
        seg0 = "0"+String(seg);
      }
      else{
        seg0 = String(seg);
      }
      mint0 = "00:";
      hor0 = "00:";
    }
    timehms = hor0+mint0+seg0;


    Debug.print("time: ");
    Debug.print(timehms);
    //Debug.print(", temSeg: ");
    //Debug.print(temSeg);
    Debug.print(", stopwatch : ");
    Debug.print(controlTime.ms()*0.001);
    Debug.print(", Ttime : ");
    Debug.print(Ttime);
    Debug.print(", AH : ");
    Debug.println(valAH);
}

void serialEvent1()
{    // Interrupción recepción serie USART
    flagbuff = false;
    rcvchar=0x00;        // Inicializo carácter recibido
    //Debug.println("EventoSerial");

    while(Serial1.available()){         // Si hay algo pendiente de recibir ...

    rcvchar=Serial1.read();    // lo descargo y ...
    comms_addcbuff(rcvchar); // lo añado al buffer y ...
  }
}
