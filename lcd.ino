#include <LCD16x2.h>


LCD16x2 lcd;


int LCDBotones(){
  return lcd.readButtons();
}

void LCDBorrar(){
  lcd.lcdClear(); 
}

void LCDEstadoGPS(bool ok){
  lcd.lcdGoToXY(1,1);
  if (ok==false)
    lcd.lcdWrite("NO GPS!");
  else
    lcd.lcdWrite("GPS OK!"); 
}

void LCDSpeedGPS(int speed){
  char txtSpeed[8];
  sprintf(txtSpeed,"%03d Kmh",speed);
  lcd.lcdGoToXY(10,1);
  lcd.lcdWrite(txtSpeed);
}

void LCDMetrosGPS(unsigned long tm,bool totales){
  char aux[17]="------ km TOTAL ";
  if (totales==false){
    sprintf(aux,"%06lu km TRIP ",tm);
  }
  else{
    sprintf(aux,"%06lu km TOTAL",tm);
  }
  
  lcd.lcdGoToXY(1,2);
  lcd.lcdWrite(aux);
}

void LCDIntroMenu(){
  LCDBorrar();
  lcd.lcdGoToXY(1,1);
  lcd.lcdWrite(" ---- CONF ---- ");
}
/*
 * 
 * 
 * 
 * buttons = lcd.readButtons();
  
  lcd.lcdGoToXY(7,1);
  if(buttons & 0x01)  
    lcd.lcdWrite("0");
  else
    lcd.lcdWrite("1");
    
  lcd.lcdGoToXY(15,1);
  if(buttons & 0x02)  
    lcd.lcdWrite("0");
  else
    lcd.lcdWrite("1");
    
  lcd.lcdGoToXY(7,2);
  if(buttons & 0x04)  
    lcd.lcdWrite("0");
  else
    lcd.lcdWrite("1");
    
  lcd.lcdGoToXY(15,2);
  if(buttons & 0x08)  
    lcd.lcdWrite("0");
  else
    lcd.lcdWrite("1");
    
  delay(70);
 * 
 * 
 * 
 */
