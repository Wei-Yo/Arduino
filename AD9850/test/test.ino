
#define W_CLK 8                        // AD9850 Module pins.    
#define FQ_UD 7       
#define DATA  6       
#define RESET 5   

#define AD9850_CLOCK 125000000         // Module crystal frequency. Tweak here for accuracy.

#define pulseHigh(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); }

String val;
long unsigned int freq = 1000;         // Set initial frequency.
long unsigned int freqOld = freq;

void setup() {
  pinMode(FQ_UD, OUTPUT);              // Configure pins for output to AD9850 module.
  pinMode(W_CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(RESET, OUTPUT);
  
  pulseHigh(RESET);
  pulseHigh(W_CLK);
  pulseHigh(FQ_UD);

  Serial.begin(9600);
  
  sendFrequency(62000000);
  
}

void loop() {

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

