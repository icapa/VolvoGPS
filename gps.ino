#define SPEED_POSITION 7
#define KMPH_PER_KNOT 1.852

/*
 *   "$GPRMC,045200.000,A,3014.3820,N,09748.9514,W,36.88,65.02,030913,,,A*77\r\n"
 */

int EEPROM_KM_TOTAL = 2;   // Total km
int EEPROM_KM_TRIP  = 6; // Trip km

int EEPROM_REV = 10;     // 

struct Revision{
  unsigned long km;
  byte day;
  byte month;
  short int year;
};

const char gprmc[]="$GPRMC";

bool isGPRMCFrame(char* frame){
  int i;
  for (i=0;i<6;i++){
    if (frame[i]!=gprmc[i]){
      return false;
    }
  }
  return true;
}

int gpsGetSpeed(char *completeFrame){
  int sizeFrame = strlen(completeFrame);
  int i,commaNumber=0;
  int initSpeed=0,endSpeed=1;
  char dato;
  char auxVel[16];
  float vel;
  for (i=0;i<sizeFrame;i++){
    /* Look for SPEED_POSITION */
    dato = completeFrame[i];
    if (dato==',') {
      commaNumber++;
      if (commaNumber==SPEED_POSITION){
        initSpeed = i;
      }
      if (commaNumber == SPEED_POSITION+1){
        endSpeed = i;
        break;
      }  
    }
    
  }
  if ((endSpeed-initSpeed)<2){
    return -1;
  }
  strncpy(auxVel,completeFrame+initSpeed+1,endSpeed-initSpeed-1);
  auxVel[endSpeed-initSpeed-1]='\0';
  vel = atof(auxVel);
  vel = vel * KMPH_PER_KNOT;
  return int(vel);
}

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
unsigned long GPSLeeKmParciales(){
   unsigned long km = 0xFFFFFFFF;
  EEPROM.get(EEPROM_KM_TRIP,km);
  return km;
}
 



