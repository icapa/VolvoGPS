int EEPROM_KM_TOTAL = 2;   // Total km
int EEPROM_KM_TRIP  = 6; // Trip km

int EEPROM_REV = 10;     // 

struct Revision{
  unsigned long km;
  byte day;
  byte month;
  short int year;
};


unsigned long GPSCalculaKm(unsigned long ms, int speed){
  /*
   * La velocidad por milisegundo :
   * (km/h) * (mS) 
   * (1000 m/h) * (1/1000) s 
   * (m * s) /(h) = (m * s)/(3600 s)
   * Distancia = (ms * speed) / 3600
   */
   
   float metros=0.0;
   metros = (speed * ms)/(3600);
   return (long int)(metros);
   
   
}

void GPSGuardaKmTotales(unsigned long km)
{
  EEPROM.put(EEPROM_KM_TOTAL,km);
}
void GPSGuardaKmParciales(unsigned long km){
  EEPROM.put(EEPROM_KM_TRIP,km);
}

unsigned long GPSLeeKmTotales()
{
  unsigned long km = 0xFFFFFFFF;
  EEPROM.get(EEPROM_KM_TOTAL,km);
  return km;
}
 



