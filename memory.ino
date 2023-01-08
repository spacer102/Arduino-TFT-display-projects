void SaveMemory (int hodnota, int tlacidko) {
  for(int i=0; i <= EEPROM.length()-1; i++) {
      if(EEPROM.read(i) == 0) {
         EEPROM.write(i, hodnota);
     }
     else if(i == EEPROM.length()-1)
       {
          displej1.setSegments(SEG_ERROR,4,0);
       }
    }
 }

void deleteMemory()
{
  for (int i = 0 ; i < EEPROM.length() ; i++) {
       EEPROM.write(i, 0);
   }
}

int LoadMemory() {
  int hodnota=0;
    for(int i=0; i < EEPROM.length()-1; i++)
  {
    if(EEPROM.read(i) != 0)
    {
      int hodnota = EEPROM.read(i);
      return hodnota;
    }
    else if(EEPROM.read(i) == 0) {
      displej1.setSegments(SEG_ERROR,4,0);
      hodnota=hodnota+1;
    }
  }
}
