#include <EEPROM.h>
#include <DMXSerial.h>
#include <TM1637Display.h>
#include <elapsedMillis.h>

elapsedMillis cas;

#define CLK A4
#define DIO A5
int b1 = 8;
int b2 = 7;
int b3 = 3;
int b4 = 1;

bool start = false;

const uint8_t SEG_ERROR[] = {
  SEG_A | SEG_F | SEG_E | SEG_D | SEG_G,           
  SEG_A | SEG_B | SEG_C | SEG_G | SEG_E | SEG_F,
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,    
  SEG_A | SEG_B | SEG_C | SEG_G | SEG_E | SEG_F         
  };

const uint8_t SEG_SAVED[] = {
  SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,
  SEG_A | SEG_F | SEG_E | SEG_D | SEG_G,
  SEG_G | SEG_E | SEG_D | SEG_B | SEG_C          
};
const uint8_t SEG_LOAD[] = {
  SEG_F | SEG_E | SEG_D,
  SEG_A | SEG_F | SEG_E | SEG_D | SEG_B | SEG_C,
  SEG_A | SEG_F | SEG_G | SEG_B | SEG_E | SEG_C,
  SEG_G | SEG_E | SEG_D | SEG_B | SEG_C  
};
const uint8_t SEG_DEMO[] = {
  SEG_G | SEG_E | SEG_D | SEG_B | SEG_C,
  SEG_A | SEG_F | SEG_G | SEG_B | SEG_E | SEG_D  
};
const uint8_t SEG_PLAY[] = {
  SEG_A | SEG_F | SEG_E | SEG_B | SEG_G,           
  SEG_D | SEG_E | SEG_F,
  SEG_A | SEG_B | SEG_C | SEG_G | SEG_D | SEG_E,    
  SEG_F | SEG_B | SEG_G | SEG_C | SEG_D    
};

TM1637Display displej1 = TM1637Display(CLK, DIO);

const int RedPin = 5; // PWM output pin for Red Light.
const int GreenPin = 9; // PWM output pin for Green Light.
const int BluePin = 6; // PWM output pin for Blue Light.

int ledState = LOW;
int chan1; 
int chan2;
int chan3; 
int chan4;
int kanal1=1; 
int test=0;

unsigned long previousMillis = 0; 
long interval = 20; 
boolean strobe = false;

#define RedDefaultLevel 255
#define GreenDefaultLevel 0
#define BlueDefaultLevel 0

void setup () {
   //Serial.begin(9600);
   displej1.setBrightness(7);
   displej1.clear();
   DMXSerial.init(DMXReceiver);
   analogWrite(RedPin, 0);

   
   DMXSerial.write(1, 80);
   DMXSerial.write(2, 0);
   DMXSerial.write(3, 0);

   pinMode(b1, INPUT);
   pinMode(b2, INPUT);
   pinMode(b3, INPUT);
    
   pinMode(RedPin, OUTPUT); 
   pinMode(GreenPin, OUTPUT);
   pinMode(BluePin, OUTPUT);
  
  displej1.setSegments(SEG_LOAD,4,0);
  chan1 = LoadMemory();
  chan2 = chan1+1;
  chan3 = chan1+2;
  chan4 = chan1+3;
  delay(1000);
  displej1.clear();
  kanal1=chan1;
  delay(100);
}


 void loop() {
   int ba = digitalRead(b1);
   int bb = digitalRead(b2);
   int bc = digitalRead(b3);
   analogWrite(RedPin, 0);
  

   test=0;
   ButtonReact();

   unsigned long lastPacket = DMXSerial.noDataSince();
   unsigned long currentMillis = millis();
   displej1.showNumberDecEx(kanal1,0,0);
  
   if (lastPacket < 5000) {
     ButtonReact();
     int strobeSpeed = DMXSerial.read(chan4);
     if (strobeSpeed > 10) {
         strobe = true;
         interval = strobeSpeed-10;
     } else {
         strobe = false;
     }

     if(strobe){
         if (currentMillis - previousMillis >= interval) {
                ButtonReact();
                previousMillis = currentMillis;
                
                if (ledState == LOW) {
                     ButtonReact();
                     ledState = HIGH;
                     analogWrite(RedPin, DMXSerial.read(chan1));
                     analogWrite(GreenPin, DMXSerial.read(chan2));
                     analogWrite(BluePin, DMXSerial.read(chan3));
                     
                } else {
                     ButtonReact();
                     ledState = LOW;
                     analogWrite(RedPin, 0);
                     analogWrite(GreenPin, 0);
                     analogWrite(BluePin, 0);
                    
                }
         }
     } else {
         ButtonReact();
         analogWrite(RedPin, DMXSerial.read(chan1));
         analogWrite(GreenPin, DMXSerial.read(chan2));
         analogWrite(BluePin, DMXSerial.read(chan3));
     }
   } 
   
   
   if((lastPacket >= 5000) && (cas>5000)){
    for(int y=0; y<25; y++) {
      bc = digitalRead(b3);
    displej1.clear();
    displej1.setSegments(SEG_DEMO,2,0);
    if(lastPacket<100) {
      break;
    }
    
    if(bc==1) {
      delay(350);
      if(start==false)
      {
        start=true;
      }
      delay(50);
      bc = digitalRead(b3);
    }
    }
    
    while(start==true) {
      displej1.setSegments(SEG_PLAY,4,0);
      elapsedMillis stroboskop;
      float timeout=500;
      int del1=0; 
      int del2=0; 
      int del3=0;
      for(int i=0; i<601; i++)
     {
       displej1.setSegments(SEG_PLAY,4,0);
        if(i<256) {
        analogWrite(RedPin, i);}
        if(i>255 && i<512) {
        analogWrite(RedPin, 255-del1);
        del1=del1+1;}
        
        if(i>50 && i<300) {
        analogWrite(GreenPin, i-50); } 
        if(i>300 && i<557) {
        analogWrite(GreenPin, 255-del2);
        del2=del2+1;}
                
        if(i>100 && i<330) {
        analogWrite(BluePin, i-100); }
        if(i>329 && i<586) {
        analogWrite(BluePin, 255-del3);
        del3=del3+1;}
        bc = digitalRead(b3);

        if((bc==1) || (DMXSerial.noDataSince()<100)) {
          start=false;
          delay(350);
          cas=0;
          i=599;
        }
        while(stroboskop<=int(timeout)+2) {
          displej1.setSegments(SEG_PLAY,4,0);
                analogWrite(RedPin, 0);
                analogWrite(GreenPin, 0);
                analogWrite(BluePin, 0);
             if((bc==1) || (DMXSerial.noDataSince()<100)) {
               start=false;
               delay(350);
               cas=0;
               i=599;}
             else if(stroboskop>int(timeout)) {
               stroboskop=0;
               timeout=timeout-1.7;
               break;} }
       }
        analogWrite(RedPin, 0);
        analogWrite(GreenPin, 0);
        analogWrite(BluePin, 0);
  }
  if(start==false) {
     cas=0;
     analogWrite(RedPin, 0);
  }
 }
}

void ButtonReact()
{
   int ba = digitalRead(b1);
   int bb = digitalRead(b2);
   int bc = digitalRead(b3);
  
    if(ba==1) {
    displej1.clear();
    kanal1=kanal1-1;
    delay(200);
    if(kanal1==0) {
      kanal1=255;
    }
    else if(kanal1==256) {
       kanal1=1;
     }
    displej1.showNumberDecEx(kanal1,0,0);
   }
   
   else if(bb==1) {
      displej1.clear();
      kanal1=kanal1+1;
      delay(200);
    if(kanal1==0) {
      kanal1=255;
    }
    else if(kanal1==256) {
       kanal1=1;
     }
      displej1.showNumberDecEx(kanal1,0,0);
    }
    
   else if(bc == 1) {
    displej1.clear();
    displej1.setSegments(SEG_SAVED,3,0);
    deleteMemory();
    SaveMemory(kanal1, bc);
    displej1.clear();
    displej1.showNumberDecEx(kanal1,0,0);
   }
}
