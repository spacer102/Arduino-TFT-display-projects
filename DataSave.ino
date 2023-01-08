
void saveData(unsigned long data, int wave){
  for(int i=0; i<EEPROM.length()-540; i++){
    EEPROM.write(i, 0);}
  int eeAdress=0;
  EEPROM.put(eeAdress, data);
  eeAdress+=sizeof(unsigned long);
  EEPROM.put(eeAdress, wave);
}

unsigned long loadFreq() {
 int eeAdress=0;
 unsigned long freq=0;
 EEPROM.get(eeAdress, freq);
 return freq;
}

int loadWave() {
  int eeAdress=0;
  int wave=0;
  eeAdress+=sizeof(unsigned long);
  EEPROM.get(eeAdress, wave);
  return wave;
}
