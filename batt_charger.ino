#include "comms.h"
#include "xmDAC.h"
//#include "StopWatch.h"
#include "Timer.h"
#include "globals.h"


char rcvchar=0x00;   // último carácter recibido
bool flagcommand=false;
bool flagbuff = false;
bool flagStep=false;
bool flagtime=true;
int count = 0;
char letter = 0;
int i=0;

double valcurrent = 0; //solo para mostrar
double valvoltage = 0;
double valtemp = 0;

StopWatch controlTime;
StopWatch SerialTime;
Control control;
Program program;
I2CEEPROM i2c_eeprom(0x50);

Timer t;
Timer to;
int LedComms = 17;

char anbu[1024];  //cfg json
char type[15][20]; //inicio, pausa,carga, fin
unsigned long duration[15]; //time
float current[15];

void setup()
{
  Debug.begin(230400);
  Serial1.begin(230400);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LedComms, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(LedComms, LOW);

//new value

/*  unsigned char data[]={0x01};

  int dat = crc16_SingleBuf(data,1);

  int dath = highByte(dat);
  Debug.println(dath);
  int datl= lowByte(dat);
  Debug.println(datl);*/

  control.begin();
  t.every(1000,printMessage); //mostrar valores
}

void loop()
{
  if(flagcommand) comms_procesa_comando();
  t.update();
  to.update();

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
    Debug.print("millis() : ");
    Debug.print(millis());
    Debug.print(", stopwatch : ");
    Debug.println(controlTime.ms());
}

void serialEvent1()
{    // Interrupción recepción serie USART

    flagbuff = false;
    rcvchar=0x00;        // Inicializo carácter recibido
    Debug.println("EventoSerial");

    while(Serial1.available()){         // Si hay algo pendiente de recibir ...

    rcvchar=Serial1.read();    // lo descargo y ...
    comms_addcbuff(rcvchar); // lo añado al buffer y ...
        //Serial.println(SerialTime.us());
  }
}
