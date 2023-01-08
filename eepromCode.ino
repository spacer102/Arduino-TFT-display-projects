#include <EEPROM.h>

struct MyObject {
  float field1;
  byte field2;
  String nieco;
};

void secondTest() {
  int eeAddress = 0; 
  MyObject customVar; 
  EEPROM.get(eeAddress, customVar);
  Serial.println("Read custom object from EEPROM: ");
  Serial.println(customVar.field1);
  Serial.println(customVar.field2);
  Serial.println(customVar.nieco);
}


void setup() {
  Serial.begin(9600);
  secondTest();
  char pica[] = "536";
  unsigned long f= (unsigned long)pica[1]-'0';
  Serial.println(f);
  char cc[5];
  String str;
  str=String(f);
  str.toCharArray(cc,5);
  Serial.println(str);

  
}

void loop() {
}
