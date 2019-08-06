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
char letter = 0;
char stepState = 0x49;
int i=0;

double valcurrent = 0; //solo para mostrar
double valvoltage = 0;
double valtemp = 0;

StopWatch controlTime;
Control control;
Program program;
//I2CEEPROM i2c_eeprom(0x50);

Timer t;
Timer to;
int LedComms = 17;
int LedRelay = 20;

char anbu[1024];  //cfg json
char type[15][20]; //inicio, pausa,carga, fin
unsigned long duration[15]; //time
float AmperH[15];
float current[15];
float maxtemp[15];
float mintemp[15];

void setup()
{
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
    Debug.print("millis() : ");
    Debug.print(millis());
    Debug.print(", stopwatch : ");
    Debug.println(controlTime.ms()*0.001);
}

void serialEvent1()
{    // Interrupción recepción serie USART
    flagbuff = false;
    rcvchar=0x00;        // Inicializo carácter recibido
    Debug.println("EventoSerial");

    while(Serial1.available()){         // Si hay algo pendiente de recibir ...

    rcvchar=Serial1.read();    // lo descargo y ...
    comms_addcbuff(rcvchar); // lo añado al buffer y ...
  }
}
