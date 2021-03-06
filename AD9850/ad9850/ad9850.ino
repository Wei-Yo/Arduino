#define W_CLK 8			// AD9850 模組 pin腳
#define FQ_UD 7			// AD9850 模組 pin腳
#define DATA  6			// AD9850 模組 pin腳
#define RESET 5	 		// AD9850 模組 pin腳

#define GAINDT A3
#define GAINCLK A2
#define DT A1
#define CLK A0
#define SW 4

#define BUTTON 9
#define G2 10
#define G1 11
#define G0 12

#define AD9850_CLOCK 125000000				 // AD9850 模組 主頻率
#define MAX_FREQ 62500000
#define MIN_FREQ 1

#define K 1000
#define M 1000000
#define G 1000000000

#define pulseHigh(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); }
#include <U8glib.h>
#include <Rotary.h>

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);	// oled控制

String strfreq,strspeace,strgain;
unsigned long starttime,buttontime;
unsigned long freq, speace;

boolean onoroff,a;

unsigned long step[] = {1, 10, 100, 1000, 10000 ,100000 ,1000000};
int x = 0;
boolean buttonpress,buttonpress2;

Rotary r = Rotary(CLK, DT);
Rotary Gainr = Rotary(GAINCLK, GAINDT);

byte ctrl[8][3] = { {0,0,0},
										{0,0,1},
										{0,1,0},
										{0,1,1},
										{1,0,0},
										{1,0,1},
										{1,1,0},
										{1,1,1},
};
float WaveGain [8] = { 0.08, 0.16, 0.32, 0.63, 1.26, 2.52, 5.01, 10};

byte Gain = 0;

float floatfreq,floatspeace;

void setup() {
	pinMode(FQ_UD, OUTPUT);		// AD9850 模組 pin腳
	pinMode(W_CLK, OUTPUT);   // AD9850 模組 pin腳
	pinMode(DATA,  OUTPUT);		// AD9850 模組 pin腳
	pinMode(RESET, OUTPUT);   // AD9850 模組 pin腳

	pinMode(SW, INPUT_PULLUP);
	pinMode(BUTTON, INPUT);
	
	pinMode(G0, OUTPUT);
	pinMode(G1, OUTPUT);
	pinMode(G2, OUTPUT);

	pulseHigh(RESET);  
	pulseHigh(W_CLK);
	pulseHigh(FQ_UD);

	Serial.begin(9600);
	
	freq = 10000;
	sendFrequency(freq);

	u8g.setFont(u8g_font_unifont); //oled

	PCICR |= (1 << PCIE1);
	PCMSK1 |= (1 << PCINT8) | (1 << PCINT9) | (1 << PCINT10) | (1 << PCINT11);
	sei();

	speace = step[x];

	u8g.setColorIndex(1);
}

void loop() {
	if (digitalRead(BUTTON) == LOW){
		if (!buttonpress2){		//按下的瞬間
			buttonpress2 = true;
			onoroff = !onoroff;
			buttontime = millis();
		}
	}else buttonpress2 = false;

	u8g.firstPage();

	do{
		if (digitalRead(SW) == LOW){
			if (!buttonpress){		//按下的瞬間
				buttonpress = true;
				x = x + 1;
				if(x == 7) x = 0;
				speace = step[x];
				starttime = millis();
			}else {
				if (millis() - starttime >500){		//按著的時候
					x = x + 1;
					if (x == 7) x = 0;
					speace = step[x];
					starttime = millis();
				}
			}
		}else buttonpress = false;

		if (freq > 0 && freq < K){
			sendFrequency(freq);
			strfreq = String(freq);
		}
		
		else if (freq >= K && freq < M){ //K
			sendFrequency(freq);
			floatfreq = freq;
			floatfreq /= K;
			strfreq = String(floatfreq);
			strfreq = strfreq + 'K';
		}
		
		else if (freq >= M && freq < G){ //M
			sendFrequency(freq);
			floatfreq = freq;
			floatfreq /= M;
			strfreq = String(floatfreq);
			strfreq = strfreq + 'M';
		}
		
		if (speace < K)strspeace = String(speace);
		else if (speace >= K && speace < M){
			floatspeace = speace;
			floatspeace /= K;
			strspeace = String(floatspeace);
			strspeace = strspeace + 'K';
		} else if (speace >= M){
			floatspeace = speace;
			floatspeace /= M;
			strspeace = String(floatspeace);
			strspeace = strspeace + 'M';
		}
		
		strgain = WaveGain[Gain];
		u8g.drawStr(0,13,"Freq:");
		u8g.drawStr(42,13,strfreq.c_str());
		u8g.drawStr(0,38,"Step:");
		u8g.drawStr(42,38,strspeace.c_str());
		u8g.drawStr(0,63,"Gain:");
		u8g.drawStr(42,63,strgain.c_str());
	}while (u8g.nextPage());

	delay(50);

}

void amplitude(byte Gain) {
	byte pin = 10;
	for (byte bol = 0; bol < 3; bol++) {
		digitalWrite(pin,ctrl[Gain][bol]);
		pin++;
	}
}

void tfr_byte(byte data) {
	for (int i = 0; i < 8; i++, data >>= 1) {
		digitalWrite(DATA, data & 0x01);
		pulseHigh(W_CLK);	 //在每個bit送出之後,CLK需要pulsed high
	}
}

void sendFrequency(double frequency) {
	int32_t freq1 = frequency * 4294967295/AD9850_CLOCK;		//AD9850 使用 125MHz的石英震盪
	for (int b = 0; b < 4; b++, freq1 >>= 8) {
		tfr_byte(freq1 & 0xFF); //取低8位元
	}
	
	if (onoroff){
		tfr_byte(0x00);	//on
		u8g.drawStr(106,38,"on");
	}
	else {
		tfr_byte(0x04);	//off
		u8g.drawStr(106,38,"off");
	}
	pulseHigh(FQ_UD);		// 好!	應該可以看到輸出了
}

ISR(PCINT1_vect) {
	unsigned char result = r.process();
	unsigned char result2 = Gainr.process();
	
	if (result == DIR_CW && freq < MAX_FREQ) {
		if ((MAX_FREQ - freq) < speace) {
			freq = MAX_FREQ;
		}
		else {
			freq += speace;
		}
	}
	else if (result == DIR_CCW && freq > MIN_FREQ) {
		if ((freq - MIN_FREQ) < speace) {
			freq = MIN_FREQ;
		}
		else {
			freq -= speace;
		}
	}
	
	if (result2 == DIR_CW) {
		if (Gain < 7) Gain ++;
		amplitude(Gain);
	}
	else if (result2 == DIR_CCW) {
		if (Gain > 0) Gain --;
		amplitude(Gain);
	}
}