int clockPin=13; //74HC595  pin 10 SHCP
int dataPin=12;   //74HC595  pin 8 DS
int latchPin=11;  //74HC595  pin 9 STCP

int d4 = 10;
int d3 = 9;
int d2 = 8;
int d1 = 7;

int ledPin = 6;
int btnPin = 5;

unsigned char digit[]= {0xd7, 0x11, 0xcd, 0x5d, 0x1b, 0x5e, 0xde, 0x15, 0xdf, 0x1f, 0xd7};
unsigned char dot = 0x20;
int number [] = {0,0,0,0};
unsigned long prevTime = 0;
unsigned long initTime = 0;
unsigned long cooldownStart = 0;
int cooldown = 500;
int systemState = 0;

void setup() {
  pinMode(latchPin,OUTPUT);
  pinMode(clockPin,OUTPUT);
  pinMode(dataPin,OUTPUT);
  pinMode(d1, OUTPUT);
  pinMode(d2, OUTPUT);
  pinMode(d3, OUTPUT);
  pinMode(d4, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(btnPin, INPUT_PULLUP);
  
  digitalWrite(d1, HIGH);
  digitalWrite(d2, HIGH);
  digitalWrite(d3, HIGH);
  digitalWrite(d4, HIGH);
}

void ledOn(){
  digitalWrite(ledPin, HIGH);
}

void ledOff(){
  digitalWrite(ledPin, LOW);
}

void Display(int v1, int v2, int v3, int v4){
  segDisp(d1, digit[v1]);
  segDisp(d2, digit[v2] | dot); 
  segDisp(d3, digit[v3]); 
  segDisp(d4, digit[v4]); 
}

void segDisp(int dispPin, unsigned char value){
  digitalWrite(latchPin,LOW);
  shiftOut(dataPin,clockPin,LSBFIRST,value);
  digitalWrite(latchPin,HIGH);
  
  digitalWrite(dispPin, LOW);
  delayMicroseconds(100);
  digitalWrite(dispPin, HIGH);
}

void incNumber(){
  ++number[3];
  for (int i = 3; i > 0; --i){
    if (number[i] > 9){
      number[i] = 0;
      ++number[i - 1];
    }
  }
  if (number[0] > 9){
    number[0] = 0;
  }
}

void resetNumber(){
  number[0] = number[1] = number[2] = number[3] = 0;
}

void updateTimer(){
  unsigned long currTime = millis();
  
  if(currTime - prevTime >= 10){
    incNumber();
    prevTime = currTime;
  }
}

void calculateTime(){
  int millisElapsed = prevTime - initTime; //12345
  
  int p1 = millisElapsed % 10000; //2345
  number[0] = (millisElapsed - p1)/10000; //1
  
  int p2 = p1 % 1000; //345
  number[1] = (p1 - p2)/1000; //2

  int p3 = p2 % 100; //45
  number[2] = (p2 - p3)/100; //3

  int p4 = p3 % 10; //5
  number[3] = (p3 - p4)/10; //4
}

void loop() {
  Display(number[0],number[1],number[2],number[3]);
  bool pressed = digitalRead(btnPin) == LOW;
  if (pressed){
    ledOn();
  } else {
    ledOff();
  }
  
  switch (systemState){
    case 0: //Ready
      if (pressed) {
        while (digitalRead(btnPin) == LOW){
          delay(1);
        }
        initTime = millis();
        resetNumber();
        systemState = 1;
      }
      break;
    case 1: //Timing
      if (pressed){
        calculateTime();
        cooldownStart = millis();
        systemState = 2;
      } else {
        updateTimer();  
      }
      break;
    case 2: //Time display
      if (millis() - cooldownStart > 1000){
        while (digitalRead(btnPin) == LOW){
          Display(number[0],number[1],number[2],number[3]);
        }
        systemState = 0;
      }
      break; 
  }
}
