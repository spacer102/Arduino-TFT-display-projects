#include <SPI.h>
#include <Rotary.h>
#include "LedControl.h"
#include <elapsedMillis.h>
#include <EEPROM.h>
#include <avr/eeprom.h>

elapsedMillis cas=0;
LedControl lc=LedControl(A2,A1,A0,1);
const int SINE = 0x2000;                
const int SQUARE = 0x2028;                 
const int TRIANGLE = 0x2002;

#define LED_SINE A5
#define LED_SQUARE A4
#define LED_TRIANGLE A3

int counter = 1;
int wave = 0;
int waveType = SINE;
int wavePin = 7;
int changePin = 4;

int freqUpPin = 2;                       
int freqDownPin = 3;
int stepUpPin = 5;
int stepDownPin = 6;

const float refFreq = 25000000.0;           

const int FSYNC = 10;                       
const int CLK = 13;                        
const int DATA = 11;

Rotary r = Rotary(freqUpPin, freqDownPin);    
Rotary i = Rotary(stepUpPin, stepDownPin);    

unsigned long freq = 1000;              
unsigned long freqOld = freq;

unsigned long incr = 1;
unsigned long oldIncr = 1;


void setup() {
  Serial.begin(9600);
  SPI.begin();
  delay(50); 
  lc.shutdown(0,false);
  lc.setIntensity(0,15);
  lc.clearDisplay(0);
  freq=loadFreq();
  wave=loadWave();

  pinMode(freqUpPin, INPUT_PULLUP);      
  pinMode(freqDownPin, INPUT_PULLUP);    
  pinMode(stepUpPin, INPUT_PULLUP);
  pinMode(stepDownPin, INPUT_PULLUP);
  pinMode(wavePin, INPUT_PULLUP);
  pinMode(changePin, INPUT_PULLUP);

  pinMode(LED_SINE, OUTPUT);
  pinMode(LED_SQUARE, OUTPUT);
  pinMode(LED_TRIANGLE, OUTPUT);

  PCICR |= (1 << PCIE2);
  PCMSK2 |= (1 << PCINT18) | (1 << PCINT19);
  sei();

  waveTypeChange();
  AD9833reset();                                
  delay(50);
  AD9833setFrequency(freq, waveType);
  updateDisplay();
}

void updateDisplay() {
  Serial.println(freq);
  format(freq);
}
void format(unsigned long value) {
  unsigned long j = 10000000;
  int zeroes=true;
  
  for (int i=7; i>=0; i--) {
    int digit = (value / j) % 10;
    if(digit!=0){
      zeroes=false;
      if(incr == j && cas>=900 && counter==0) {
        lc.setChar(0,i,' ',false);
        lc.setChar(0,i,'_',false);
        cas=0;
      }
      else if((incr == j && cas<900) || (incr!=j)) {
        lc.setChar(0,i,' ',false);
        lc.setDigit(0,i,digit,false);
      }
    }
    if(digit==0 && zeroes==false) {
      if(incr == j && cas>=900 && counter==0) {
        lc.setChar(0,i,' ',false);
        lc.setChar(0,i,'_',false);
        cas=0; 
      }
      else if((incr == j && cas<900) || (incr!=j)) {
        lc.setChar(0,i,' ',false);
        lc.setDigit(0,i,digit,false);
      }
    }
    if(digit==0 && zeroes==true) {
      lc.setChar(0,i,' ',false);
      if(incr == j && counter==0)  lc.setChar(0,i,'_',false);
    }
    if(cas>1000) {
        cas=0;
     }
    j /= 10;
  }
}

void loop() {
  if (oldIncr != incr) {
    updateDisplay();
    oldIncr= incr;
   }

    unsigned char result = i.process();
  if (result) {
    if (result == DIR_CW)  {if (incr < 10000000) incr *= 10;}
    if (result == DIR_CCW) {if (incr >= 10) incr /= 10;}
    updateDisplay();
   }

  if(digitalRead(changePin) == LOW) {
    saveData(freq, wave); 
    counter>=1 ? counter=0: counter=1;
   }

  if (digitalRead(wavePin) == LOW) {
    wave += 1;
    waveTypeChange();
   }
  
  if (freq != freqOld) {                    
    AD9833setFrequency(freq, waveType);
    updateDisplay();
    freqOld = freq;                         
   }

   if(cas>800) {
    updateDisplay();
   }
}


void waveTypeChange(){
    if (wave > 2) wave = 0;
    switch (wave) {
      case 0: waveType = SINE; analogWrite(LED_SINE, 255); analogWrite(LED_SQUARE, 0); analogWrite(LED_TRIANGLE, 0); Serial.println("BLUE") ;break;
      case 1: waveType = SQUARE; analogWrite(LED_SINE, 0); analogWrite(LED_SQUARE, 255); analogWrite(LED_TRIANGLE, 0); Serial.println("RED");break;
      case 2: waveType = TRIANGLE; analogWrite(LED_SINE, 0); analogWrite(LED_SQUARE, 0); analogWrite(LED_TRIANGLE, 255); Serial.println("GREEN");break;
    }    
    AD9833setFrequency(freq, waveType);     
    updateDisplay();
    delay(200);
}

void AD9833reset() {
  WriteRegister(0x100);
  delay(10);
}

void AD9833setFrequency(long frequency, int Waveform) {
  long FreqWord = (frequency * pow(2, 28)) / refFreq;
  int MSB = (int)((FreqWord & 0xFFFC000) >> 14);   
  int LSB = (int)(FreqWord & 0x3FFF);
  LSB |= 0x4000;
  MSB |= 0x4000; 
  WriteRegister(0x2100);   
  WriteRegister(LSB);                 
  WriteRegister(MSB);               
  WriteRegister(0xC000);            
  WriteRegister(Waveform);
}

void WriteRegister(int dat) {
  SPI.setDataMode(SPI_MODE2);
  digitalWrite(FSYNC, LOW);
  delayMicroseconds(10);
  SPI.transfer(highByte(dat));
  SPI.transfer(lowByte(dat));
  digitalWrite(FSYNC, HIGH);
}

ISR(PCINT2_vect) {
 if(counter==0) {
  unsigned char result;
  result = r.process();
  if (result) {
    if (result == DIR_CW) {              
       if ((freq + incr) < 12500000) freq+=incr;
       
    } else {
        if (freq > incr) {                 
          freq -= incr;                  
        } else {                              
          if (freq >= 1) incr /= 10;
          if (incr < 1) incr = 1;           
        }  
    }
  }
 }
}
