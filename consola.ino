char consoleInput[64];
int indiceConsola=0;
bool comandoConsola=false;

void Consola(){
  while (Serial.available()){
    char dato = Serial.read();
    consoleInput[indiceConsola]=dato;
    if (dato=='\r' || dato=='\n'){
      comandoConsola=true;
      indiceConsola=false;
      
    }
    else{
      if (indiceConsola<63){
        indiceConsola++;
      }
      else{
        indiceConsola=0;
        comandoConsola=false;
        consoleInput[0]='\0';
      }
    }

    if (comandoConsola==true){
      comandoConsola=false;
      if (strncmp(consoleInput,"KM=",3)==0){
        unsigned long kmT = 0xFFFFFFFF;
        sscanf(consoleInput+3,"%ld",&kmT);
        if (kmT!=0xFFFFFFFF){
          totalMetros = kmT * 1000;
          GPSGuardaKmTotales(totalMetros);
          Serial.print("OK KM=");
          Serial.println(totalMetros);
        }
        else{
          Serial.println(consoleInput);
          Serial.println(kmT);
          Serial.println("ERROR");
        }
      }
      if (strncmp(consoleInput,"KM?",3)==0){
        Serial.print("KM=");
        Serial.println((unsigned long)totalMetros/1000);
      }
      consoleInput[0]='\0';
    }
  }
  
}

