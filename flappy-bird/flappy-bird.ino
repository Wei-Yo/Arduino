#include <SPI.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

int y_val,y,y_data;
int score = 0;
boolean start,easy,middle,hard,select,rst,gamest = 0;

unsigned int ran,cds;
const byte xsy[8] = {1,2,3,4,5,6,7,8};
const byte ysy[8] = {1,2,4,8,16,32,64,128};
const byte ylevel[6] = {248,241,227,199,143,31};
//byte buf[8]={0,0,0,0,0,0,0,0};

byte g_bright;
byte r_bright;
byte b_bright;

const byte noop = 0x0;
const byte decod = 0x9;
const byte inten = 0xa;
const byte scanl = 0xb;
const byte shutd = 0xc;
const byte displ = 0xf;

void max7219 (const byte reg,const byte data){
  digitalWrite(SS,LOW);
  SPI.transfer(reg);
  SPI.transfer(data);
  digitalWrite(SS,HIGH);
}

void setup() {
Serial.begin(9600);
pinMode(12,INPUT);	//power-光遮斷
pinMode(4,INPUT);   //easy- 
pinMode(7,INPUT);	//middle- 
pinMode(8,INPUT);	//hard - 
pinMode(2,INPUT);	//select -button
pinMode(9,INPUT);	//reset
pinMode(5,OUTPUT);	//Green		level-----easy
pinMode(3,OUTPUT);	//Red		level-----hard
pinMode(6,OUTPUT);	//Blue		level-----middle
pinMode(A0,INPUT);	//cds select

lcd.begin(16, 2);      // 初始化 LCD，一行 16 的字元，共 2 行，預設開啟背光
lcd.backlight();
lcd.clear();

SPI.begin();

max7219(scanl,7);
max7219(decod,0);
max7219(inten,8);
max7219(displ,0);
max7219(shutd,1);

for(byte i =0 ; i < 8; i++){
    max7219(i+1,0);
	}
  
}

void loop() {
  start = !digitalRead(12);
  
  if(start == 0){
    //Serial.println("Please remove the card to start the game");
	
	//lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("remove card to");
	lcd.setCursor(0, 1);
	lcd.print("start the game");
 }
  if(start == 1){
		lcd.setCursor(0, 0);
		lcd.print("~~~~~Flappy~~~~~");
		lcd.setCursor(0, 1);
		lcd.print("~~~~~~bird~~~~~~");
    selectlevel();
  }
}
//123456
void selectlevel(){
	
    //Serial.println("~flappy bird~");
    
	color();
	
	gamest = 0;
	score = 0;
	Serial.println(cds);
	//Serial.println(cds);
      /*Serial.print("easy");
      Serial.println(easy);
      Serial.print("midd");
      Serial.println(middle);
      Serial.print("hard");
      Serial.println(hard);
      Serial.println(start);*/
      while(cds > 85 && cds < 100){     //level-----easy
        start = !digitalRead(12);
        select = !digitalRead(2);
        Serial.println("you select---easy");
        Serial.println("yes or no");
		lcd.setCursor(0, 0);
		lcd.print("******easy******");
		lcd.setCursor(0, 1);
        lcd.print("***yes or no ***");
		analogWrite(5,g_bright); //93
		analogWrite(3,r_bright); // 110
		analogWrite(6,b_bright); // 75
		r_bright = 0;
		g_bright = 0;
		b_bright = 0;
        while(select == 1){                           // start game			
			Serial.println("game start");
		if(gamest == 0){
			lcd.setCursor(0, 0);
			lcd.print("***game start***");
			lcd.setCursor(0, 1);
			lcd.print("                ");
			}
			rst = !digitalRead(9);
			game3();
			//break;
          if(rst == 0 || start == 0)break;
        }
        //Serial.println(start);
		rst = !digitalRead(9);
        if(rst == 0 || start == 0)break;
      }
      while(cds > 100 && cds < 120){     //level-----middle
        start = !digitalRead(12);
        select = !digitalRead(2);
        Serial.println("you select---middle");
        Serial.println("yes or no");
		lcd.setCursor(0, 0);
		lcd.print("*****middle*****");
		lcd.setCursor(0, 1);
        lcd.print("***yes or no ***");
		analogWrite(5,g_bright); 
		analogWrite(3,r_bright); 
		analogWrite(6,b_bright); 
		r_bright = 0;
		g_bright = 0;
		b_bright = 0;
        while(select == 1){                           //start game			
			Serial.println("game start");
		if(gamest == 0){
			lcd.setCursor(0, 0);
			lcd.print("***game start***");
			lcd.setCursor(0, 1);
			lcd.print("                ");
			}
			rst = !digitalRead(9);
			game2();
			
          if(rst == 0 || start == 0)break;
        }
        //Serial.println(start);
        rst = !digitalRead(9);
        if(rst == 0 || start == 0)break;
      }
      while( cds > 65 && cds < 85 ){     //level-----hard
        start = !digitalRead(12);
        select = !digitalRead(2);
        Serial.println("you select---hard");
        Serial.println("yes or no");
		lcd.setCursor(0, 0);
		lcd.print("******hard******");
		lcd.setCursor(0, 1);
        lcd.print("***yes or no ***");
		analogWrite(5,g_bright); //88
		analogWrite(3,r_bright); //77
		analogWrite(6,b_bright); //102
		r_bright = 0;
		g_bright = 0;
		b_bright = 0;
        while(select == 1){                           //start game
			Serial.println("game start");
			if(gamest == 0){
			lcd.setCursor(0, 0);
			lcd.print("***game start***");
			lcd.setCursor(0, 1);
			lcd.print("                ");
			}
			rst = !digitalRead(9);
			game1();
			//break;
          if(rst == 0 || start == 0)break;
        }
        //Serial.println(start);
        rst = !digitalRead(9);
        if(rst == 0 || start == 0)break;
      }

}

void color(){
	 cds = analogRead(A0);
	easy = !digitalRead(4);  //G
    middle = !digitalRead(7);  //B
    hard = digitalRead(8);  //R
	analogWrite(5,g_bright); //88
	analogWrite(3,r_bright); //77
	analogWrite(6,b_bright); //102
	if (easy == 0){
		r_bright = 0;
		g_bright = 255;
		b_bright = 0;
	}
	
	if (middle == 0){
		r_bright = 0;
		g_bright = 0;
		b_bright = 255;
	}
	
	if (hard == 0){
		r_bright = 255;
		g_bright = 0;
		b_bright = 0;
	}
}

void ctrly(){
    y_val = analogRead(A1);

    if(y_val<300) y = -1;
    else if(y_val>700) y =  1;
    else y = 0;

    if(y == 1){
      y_data ++;
      delay(10);
    }
    if(y == -1){
      y_data --;
      delay(10);
    }    

    if(y_data>7) y_data=7;
    if(y_data<0) y_data=0;
    max7219(xsy[0],ysy[y_data]);
    delay(10);
}

void game1(){
      randomSeed(analogRead(A5)); 
      ran = random(0,6);
      if(gamest == 0){
        for(int x = 7 ; x > -0.1 ; x--){
          ctrly();
          max7219(xsy[x],ylevel[ran]);
          if(x < 7)max7219(xsy[x+1],0);
          delay(60);
          if(x == 0 && (y_data==0 || y_data==1 || y_data==2) && ran == 0){
            score++;
          }else if(x == 0 && (y_data!=0 || y_data!=1 || y_data!=2) && ran == 0){
            gamest = 1;
            break;
          }
      
          if(x == 0 && (y_data==1 || y_data==2 || y_data==3) && ran == 1){
            score++;
          }else if(x == 0 && (y_data!=1 || y_data!=2 || y_data!=3) && ran == 1){
            gamest = 1;
            break;
          }
      
          if(x == 0 && (y_data==2 || y_data==3 || y_data==4) && ran == 2){
            score++;
          }else if(x == 0 && (y_data!=2 || y_data!=3 || y_data!=4) && ran == 2){
            gamest = 1;
            break;
          }
      
          if(x == 0 && (y_data==3 || y_data==4 || y_data==5) && ran == 3){
            score++;
          }else if(x == 0 && (y_data!=3 || y_data!=4 || y_data!=5) && ran == 3){
            gamest = 1;
            break;
          }
      
          if(x == 0 && (y_data==4 || y_data==5 || y_data==6) && ran == 4){
            score++;
          }else if(x == 0 && (y_data!=4 || y_data!=5 || y_data!=6) && ran == 4){
            gamest = 1;
            break;
          }
      
          if(x == 0 && (y_data==5 || y_data==6 || y_data==7) && ran == 5){
            score++;
          }else if(x == 0 && (y_data!=5 || y_data!=6 || y_data!=7) && ran == 5){
            gamest = 1;
            break;
          }
        }
      }
	  if(gamest == 1){
        Serial.println(score);
			lcd.setCursor(0, 0);
		lcd.print("your bird died..");
		lcd.setCursor(0, 1);
        lcd.print("your score : ");
		lcd.print(score);
	  }
}

void game2(){
      randomSeed(analogRead(A5)); 
      ran = random(0,6);
      if(gamest == 0){
        for(int x = 7 ; x > -0.1 ; x--){
          ctrly();
          max7219(xsy[x],ylevel[ran]);
          if(x < 7)max7219(xsy[x+1],0);
          delay(100);
          if(x == 0 && (y_data==0 || y_data==1 || y_data==2) && ran == 0){
            score++;
          }else if(x == 0 && (y_data!=0 || y_data!=1 || y_data!=2) && ran == 0){
            gamest = 1;
            break;
          }
      
          if(x == 0 && (y_data==1 || y_data==2 || y_data==3) && ran == 1){
            score++;
          }else if(x == 0 && (y_data!=1 || y_data!=2 || y_data!=3) && ran == 1){
            gamest = 1;
            break;
          }
      
          if(x == 0 && (y_data==2 || y_data==3 || y_data==4) && ran == 2){
            score++;
          }else if(x == 0 && (y_data!=2 || y_data!=3 || y_data!=4) && ran == 2){
            gamest = 1;
            break;
          }
      
          if(x == 0 && (y_data==3 || y_data==4 || y_data==5) && ran == 3){
            score++;
          }else if(x == 0 && (y_data!=3 || y_data!=4 || y_data!=5) && ran == 3){
            gamest = 1;
            break;
          }
      
          if(x == 0 && (y_data==4 || y_data==5 || y_data==6) && ran == 4){
            score++;
          }else if(x == 0 && (y_data!=4 || y_data!=5 || y_data!=6) && ran == 4){
            gamest = 1;
            break;
          }
      
          if(x == 0 && (y_data==5 || y_data==6 || y_data==7) && ran == 5){
            score++;
          }else if(x == 0 && (y_data!=5 || y_data!=6 || y_data!=7) && ran == 5){
            gamest = 1;
            break;
          }
          /*if (score == 100){
            Serial.println("level3");
            score = 0;
            game1();
          }*/
        }
      }
        Serial.println(score);
		if(gamest == 1){
        Serial.println(score);
			lcd.setCursor(0, 0);
		lcd.print("your bird died..");
		lcd.setCursor(0, 1);
        lcd.print("your score : ");
		lcd.print(score);
	  }
}

void game3(){
      randomSeed(analogRead(A5)); 
      ran = random(0,6);
      if(gamest == 0){
        for(int x = 7 ; x > -0.1 ; x--){
          ctrly();
          max7219(xsy[x],ylevel[ran]);
          if(x < 7)max7219(xsy[x+1],0);
          delay(150);
          if(x == 0 && (y_data==0 || y_data==1 || y_data==2) && ran == 0){
            score++;
          }else if(x == 0 && (y_data!=0 || y_data!=1 || y_data!=2) && ran == 0){
            gamest = 1;
            break;
          }
      
          if(x == 0 && (y_data==1 || y_data==2 || y_data==3) && ran == 1){
            score++;
          }else if(x == 0 && (y_data!=1 || y_data!=2 || y_data!=3) && ran == 1){
            gamest = 1;
            break;
          }
      
          if(x == 0 && (y_data==2 || y_data==3 || y_data==4) && ran == 2){
            score++;
          }else if(x == 0 && (y_data!=2 || y_data!=3 || y_data!=4) && ran == 2){
            gamest = 1;
            break;
          }
      
          if(x == 0 && (y_data==3 || y_data==4 || y_data==5) && ran == 3){
            score++;
          }else if(x == 0 && (y_data!=3 || y_data!=4 || y_data!=5) && ran == 3){
            gamest = 1;
            break;
          }
      
          if(x == 0 && (y_data==4 || y_data==5 || y_data==6) && ran == 4){
            score++;
          }else if(x == 0 && (y_data!=4 || y_data!=5 || y_data!=6) && ran == 4){
            gamest = 1;
            break;
          }
      
          if(x == 0 && (y_data==5 || y_data==6 || y_data==7) && ran == 5){
            score++;
          }else if(x == 0 && (y_data!=5 || y_data!=6 || y_data!=7) && ran == 5){
            gamest = 1;
            break;
          }
          /*if (score == 100){
            Serial.println("level2");
            score = 0;
            game2();
          }*/
        }
      }
        Serial.println(score);
		if(gamest == 1){
        Serial.println(score);
			lcd.setCursor(0, 0);
		lcd.print("your bird died..");
		lcd.setCursor(0, 1);
        lcd.print("your score : ");
		lcd.print(score);
	  }
}


