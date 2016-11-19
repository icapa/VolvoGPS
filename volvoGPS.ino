#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <EEPROM.h>

#define BAUDRATE 9600
#define SERIAL Serial
#define UMBRAL_SPEED_MAX 40
#define UMBRAL_SPEED_MIN 5

// GSP object
TinyGPSPlus gps;



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

// To remove compiling error
void Consola()  __attribute__((__optimize__("O2")));



void setup() {
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
  if (consoleMode==false){
    while(SERIAL.available()>0)
      gps.encode(SERIAL.read());
  }
  else
  {
    Consola();
  }
  
  
  // GPS speed timer
  if (millis()>=timerMuestreoGPS){
    timeNow=millis();
    timerMuestreoGPS = timeNow+1000;
    
    if (gps.speed.isValid()){
      estadoGPS=true;
      gpsSpeed=(int)gps.speed.kmph();         
    }else{
      estadoGPS=false;
    }
    
    metros = GPSCalculaKm(1000, gpsSpeed);
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
  if (buttons != 0x0F){
    Serial.println(buttons);
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
        Serial.println("VolvoGPS Console");
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
      delay(60);
    }
  }
  
  
}
