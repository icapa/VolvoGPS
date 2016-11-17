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

// To Save EEPROM 
bool eepromSave = false;


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
}

void loop() {
  
  // GPS character read, nonblock 
  
  while(SERIAL.available()>0)
    gps.encode(SERIAL.read());
  
  
  
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
          
    LCDSpeedGPS(gpsSpeed);
    LCDMetrosGPS((unsigned long)(totalMetros/1000));
  }

  
  // GPS status monitor
  if (estadoGPS != estadoGPSPrev){
      LCDEstadoGPS(estadoGPS);
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

  // Consola de comandos
  Consola();
  
}
