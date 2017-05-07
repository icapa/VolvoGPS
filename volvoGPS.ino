#include <SoftwareSerial.h>
#include <Wire.h>
#include <EEPROM.h>

#define BAUDRATE 9600
#define SERIAL Serial
#define UMBRAL_SPEED_MAX 40
#define UMBRAL_SPEED_MIN 5


bool estadoGPS=false;
bool estadoGPSPrev = true;

// Timers
unsigned long timeNow;
unsigned long timerGps;
unsigned long timerMuestreoGPS;


// GPS Speed
int gpsSpeed=0;

// Distance & km
unsigned long metros=0;
unsigned long totalMetros=0;
unsigned long tripMetros=0;
bool mostrarTotales=true;

// To Save EEPROM 
bool eepromSave = false;

// Console mode
bool consoleMode = false;
int buttons=0;
int buttonsOld=0;
int buttonsCount=0;

// To remove compiling error
void Consola()  __attribute__((__optimize__("O2")));

// LED
int ledPin1=13;
int ledPin2=9;

bool led1Status=false;

bool gpsValid=false;
int stepsNoGps=0;

void setup() {
  pinMode(ledPin1,OUTPUT);
  pinMode(ledPin2,OUTPUT);
  
  Wire.begin();
  Serial.begin(BAUDRATE);
  while(!Serial)
  LCDBorrar();
  LCDEstadoGPS(false);
  LCDSpeedGPS(gpsSpeed);
  timerMuestreoGPS = millis()+1000;

  totalMetros = GPSLeeKmTotales();
  if (totalMetros == 0xFFFFFFFF)
    totalMetros=0;
  tripMetros = GPSLeeKmParciales();
  if (tripMetros == 0xFFFFFFFF)
    tripMetros=0;
}

void loop() {

  // GPS or Console
  /*
  if (consoleMode==false){
    while(SERIAL.available()>0)
      gpsValid = true;
      
  }
  else
  {
    Consola();
  }
  */
  // GPS speed timer
  if (millis()>=timerMuestreoGPS){
    timeNow=millis();
    timerMuestreoGPS = timeNow+2000;
    /* LED STATUS */
    if (led1Status==false){
      led1Status=true;
      digitalWrite(ledPin2,LOW);
      
    }
    else{
      led1Status=false;
      digitalWrite(ledPin2,HIGH);
      
    }

    
    
    if (gpsValid==true){ 
      gpsValid=false;
      digitalWrite(ledPin1,HIGH);
      estadoGPS=true;
      stepsNoGps=0;   
    }else{
      stepsNoGps++;
      if (stepsNoGps==10){
        stepsNoGps=0;
        digitalWrite(ledPin1,LOW);
        estadoGPS=false;
      }
      
    }

    
    metros = GPSCalculaKm(2000, gpsSpeed);
    totalMetros = totalMetros + metros;
    tripMetros = tripMetros + metros;
    if (consoleMode==false){      
      LCDSpeedGPS(gpsSpeed);
      if (mostrarTotales==true)
        LCDMetrosGPS((unsigned long)(totalMetros/1000),mostrarTotales);
      else
        LCDMetrosGPS((unsigned long)(tripMetros/1000),mostrarTotales);
    }
  }

  
  // GPS status monitor
  if (estadoGPS != estadoGPSPrev){
      if (consoleMode==false){
        LCDEstadoGPS(estadoGPS);
      }
      estadoGPSPrev=estadoGPS;
   }
  
  // Manage Store KM in EEPROM to save cycles
  if (eepromSave == false){
    if (gpsSpeed > UMBRAL_SPEED_MAX){
      eepromSave = true;
    }
  }else{
    if (gpsSpeed < UMBRAL_SPEED_MIN){
      GPSGuardaKmTotales(totalMetros);
      GPSGuardaKmParciales(tripMetros);
      eepromSave = false;
    }
  }

  //UI
  
  buttons=LCDBotones();
  if (buttons==buttonsOld){
    buttonsCount++;
    if (buttonsCount = 200000){
      buttonsCount=0;  
      
      if (buttons != 0x0F){
        if (consoleMode==false){
          if (buttons==0x0E){
            mostrarTotales = !mostrarTotales;
            if (mostrarTotales==true)
              LCDMetrosGPS((unsigned long)(totalMetros/1000),mostrarTotales);
            else
              LCDMetrosGPS((unsigned long)(tripMetros/1000),mostrarTotales);
          }
          else if (buttons==0x07){
            if (mostrarTotales==false){
              tripMetros=0;
              LCDMetrosGPS((unsigned long)(tripMetros/1000),mostrarTotales);
              GPSGuardaKmParciales(tripMetros);
            }
          }
          else{
            consoleMode=true;
            //Serial.println("VolvoGPS Console");
            LCDIntroMenu();
          }
        }
        else{
          // Gestionar los menus
          if (buttons==0x07){
            consoleMode=false;
            LCDBorrar();
            LCDEstadoGPS(estadoGPS);
          }
        }
      }
    }else{
      buttonsCount=0;
    }
  }else{
    buttonsOld=buttons;
  }
}
/* Serial port IRQ??? */
void serialEvent(){
  static int index=0;
  char completeFrame[128];
  int gpsVel;
  char dato;
  if (consoleMode==false){
    while(SERIAL.available()){
      dato = SERIAL.read();
      completeFrame[index]=dato;
      index++;
      if (index>127) index=0;
      if (dato=='\n'){
        completeFrame[index]='\0';
        index=0;
        /* Test */
        if (isGPRMCFrame(completeFrame)==true){
          gpsVel = gpsGetSpeed(completeFrame);
          if (gpsVel>=0){
            gpsValid=true;
            gpsSpeed=gpsVel;
          }
        }
      }
    }   
  }else{
    Consola();
  }
}

