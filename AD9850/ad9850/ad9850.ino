#define W_CLK 8                        // AD9850 Module pins.    
#define FQ_UD 7       
#define DATA  6       
#define RESET 5   
#define SW 4
#define DT 3
#define CLK 2

#define AD9850_CLOCK 125000000         // Module crystal frequency. Tweak here for accuracy.

#define pulseHigh(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); }

#include "U8glib.h"
#include <Rotary.h>

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);	// Display which does not send AC

String strfreq,strspeace;
unsigned long freq,speace,starttime;
//long unsigned int freqOld = freq;

unsigned long step[] = {1, 10, 100, 1000, 10000 ,100000 ,1000000};
int x = 0;
boolean buttonpress;

Rotary r = Rotary(CLK, DT);

void setup() {
  pinMode(FQ_UD, OUTPUT);              // Configure pins for output to AD9850 module.
  pinMode(W_CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(RESET, OUTPUT);
  
  pinMode(SW, INPUT_PULLUP);
  
  pulseHigh(RESET);
  pulseHigh(W_CLK);
  pulseHigh(FQ_UD);

  Serial.begin(9600);
  freq = 10000;
  sendFrequency(freq);
  
  u8g.setFont(u8g_font_unifont); //oled
  
  PCICR |= (1 << PCIE2);
  PCMSK2 |= (1 << PCINT18) | (1 << PCINT19);
  sei();
	
  speace = step[x];  
  
  u8g.setColorIndex(1);
}

void loop() {
	u8g.firstPage();
  
  do{
    if(digitalRead(SW) == LOW){
      
      if(!buttonpress){               //按下的瞬間
        buttonpress = true;
        x = x + 1;
        if(x == 7) x = 0;
        speace = step[x];
        
        starttime = millis();
      }else {
        if(millis() - starttime >500){ //按著的時候
          x = x + 1;
          if(x == 7) x = 0;
          speace = step[x];
          
          starttime = millis();
        }
      }      
    }else buttonpress = false;
    
    if(freq>0){
			sendFrequency(freq);
			strfreq = String(freq);
      strspeace = String(speace);
    }
    u8g.drawStr(0,13,"Signal generator");
    u8g.drawStr(0,38,"Step:");
    u8g.drawStr(42,38,strspeace.c_str());
    u8g.drawStr(0,63,"Hz : ");
		u8g.drawStr(42,63,strfreq.c_str());
	}while(u8g.nextPage());

  delay(50);
	
	
}

void carry() {
	
}



void tfr_byte(byte data) {
  for (int i = 0; i < 8; i++, data >>= 1) {
    digitalWrite(DATA, data & 0x01);
    pulseHigh(W_CLK);   //after each bit sent, CLK is pulsed high
  }
}

void sendFrequency(double frequency) {
  int32_t freq1 = frequency * 4294967295/AD9850_CLOCK;  // note 125 MHz clock on 9850
  for (int b = 0; b < 4; b++, freq1 >>= 8) {
    tfr_byte(freq1 & 0xFF);
  }
  tfr_byte(0x000);                     // Final control byte, all 0 for 9850 chip
  pulseHigh(FQ_UD);                    // Done!  Should see output
}

ISR(PCINT2_vect) {
  unsigned char result = r.process();

  if (result == DIR_NONE) {
		//do nothing
  }
  else if (result == DIR_CW && freq < 62500000) {
    freq = freq + speace;
    if (freq > 62500000) freq = 62500000;
  }
  else if (result == DIR_CCW && freq > 1) {
    freq = freq - speace;
    if (freq < 1) freq = 1;
  }
}