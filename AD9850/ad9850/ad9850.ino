#define W_CLK 8			// AD9850 模組 pin腳
#define FQ_UD 7			// AD9850 模組 pin腳
#define DATA  6			// AD9850 模組 pin腳
#define RESET 5	 		// AD9850 模組 pin腳
#define SW 4
#define DT 3
#define CLK 2
#define BUTTON 9
#define G2 10
#define G1 11
#define G0 12

#define AD9850_CLOCK 125000000				 // AD9850 模組 主頻率
#define MAX_FREQ 62500000
#define MIN_FREQ 1

#define pulseHigh(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); }
#include <U8glib.h>
#include <Rotary.h>

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);	// oled控制

String strfreq,strspeace;
unsigned long starttime,buttontime;
unsigned long freq, speace;

boolean onoroff,a;

unsigned long step[] = {1, 10, 100, 1000, 10000 ,100000 ,1000000};
int x = 0;
boolean buttonpress,buttonpress2;

Rotary r = Rotary(CLK, DT);

byte ctrl[8][3] = { {0,0,0},
										{0,0,1},
										{0,1,0},
										{0,1,1},
										{1,0,0},
										{1,0,1},
										{1,1,0},
										{1,1,1},
};

String plus;
byte Gain,bol;
int k,y;

void setup() {
	pinMode(FQ_UD, OUTPUT);		// AD9850 模組 pin腳
	pinMode(W_CLK, OUTPUT);   // AD9850 模組 pin腳
	pinMode(DATA,  OUTPUT);		// AD9850 模組 pin腳
	pinMode(RESET, OUTPUT);   // AD9850 模組 pin腳

	pinMode(SW, INPUT_PULLUP);
	pinMode(BUTTON, INPUT);
	
	pinMode(G0,OUTPUT);
	pinMode(G1,OUTPUT);
	pinMode(G2,OUTPUT);

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
	if (Serial.available()){
		plus = Serial.readString();
		if (plus == "+"){
			Gain ++;
			if (Gain > 7) Gain = 7;
			amplitude(Gain);
			Serial.println(Gain);
		}
		else if (plus == "-"){
			Gain --;
			if (Gain == 255) Gain = 0;
			amplitude(Gain);
			Serial.println(Gain);
		}
	}
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

		if (freq>0){
			sendFrequency(freq);
			strfreq = String(freq);
			strspeace = String(speace);
		}
		u8g.drawStr(0,13,"Signal generator");
		u8g.drawStr(0,38,"Step:");
		u8g.drawStr(42,38,strspeace.c_str());
		u8g.drawStr(0,63,"Hz : ");
		u8g.drawStr(42,63,strfreq.c_str());
	}while (u8g.nextPage());

	delay(50);

}

void carry() {

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

ISR(PCINT2_vect) {
	unsigned char result = r.process();

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
}

/*ISR(PCINT2_vect) {
	unsigned char result = r.process();

	if (result == DIR_CW) {
			Gain ++;
			if (Gain > 7) Gain = 7;
			amplitude(Gain);
	}
	else if (result == DIR_CCW) {
			Gain --;
			if (Gain = 255) Gain = 0;
			amplitude(Gain);
	}
}*/