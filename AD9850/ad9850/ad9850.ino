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

String strfreq,kfreq;
unsigned long freq;         // Set initial frequency.
//long unsigned int freqOld = freq;

int step[] = {1, 10, 100, 1000, 10000 ,100000 ,1000000};

Rotary r = Rotary(CLK, DT);

void setup() {
  pinMode(FQ_UD, OUTPUT);              // Configure pins for output to AD9850 module.
  pinMode(W_CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(RESET, OUTPUT);
  
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
	
}

void loop() {
	u8g.firstPage();
  do{  
			/*delay(1000);
			int x = x + 1;
			int spacing = step[x];
			Serial.println(spacing);*/
    if(freq>0){
			sendFrequency(freq);
			strfreq = String(freq);
    }
    u8g.setColorIndex(1);
    u8g.drawStr(0,22,"Signal generator");
    u8g.drawStr(0,55,"Hz : ");
		u8g.drawStr(42,55,strfreq.c_str());
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
    freq = freq + 1000;
  }
  else if (result == DIR_CCW && freq > 1) {
    freq = freq - 1000;
  }
}