#include <Thread.h> 
#include <LiquidCrystal.h>
#include <TimerOne.h>
#define size 20

#define MESSAGE '0'
#define WAY '1'
#define END '2'

#define ONE_METER '0'
#define TURN45 '1'
#define TURN90 '2'
#define TURN_BACK45 '3'
#define TURN_BACK90 '4'

Thread driveThread = Thread(); 
Thread bluetoothListenerThread = Thread();
LiquidCrystal lcd(5, 6, 7, 8, 9, 10);

// первый двигатель
int enA = A0;
int in1 = A1;
int in2 = A2;

// второй двигатель
int enB = A5;
int in3 = A3;
int in4 = A4;

int i = 0;
int seconds = 0;
String command = "";
bool iRead = false;
bool isEnd = false;
bool isWay = false;
bool isMessage = false;

void letsGo(String);
void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  Serial.println("Ready to srart");
  displayOnLCD("Ready to start");
  Timer1.initialize();
  Timer1.attachInterrupt(addSecond);

  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  
 //driveThread.onRun(letsGo());
 //driveThread.setInterval(1000);

  bluetoothListenerThread.onRun(lisenBluetoothStream);
  bluetoothListenerThread.setInterval(20);
  
 //Serial.println("Go");
  //int start = seconds;
  //digitalWrite(in1, HIGH);
  //digitalWrite(in2, LOW);

  //digitalWrite(in3, HIGH);
  //digitalWrite(in4, LOW);
  
  //analogWrite(enA, 50);
  //analogWrite(enB, 50);
  //while(seconds - start < 30);
  //Serial.println("Stop"); 

  //digitalWrite(in1, LOW);
  //digitalWrite(in2, LOW);
  //digitalWrite(in3, LOW);
  //digitalWrite(in4, LOW);
  
}

int recognice(char ch){
  if(ch == '1')
    return 1;
    
  if(ch == '2')
    return 2;
    
  if(ch == '3')
    return 3;
    
  if(ch == '4')
    return 4;
    
  if(ch == '5')
    return 5;
    
  if(ch == '6')
    return 6;
    
  if(ch == '7')
    return 7;
    
  if(ch == '8')
    return 8;
    
  if(ch == '9')
    return 9;
    
  if(ch == '0')
    return 0;

  return 10;
}

void loop() {
  //if (driveThread.shouldRun())
   //     driveThread.run();
    
  if (bluetoothListenerThread.shouldRun() && !isWay)
      bluetoothListenerThread.run();
}

void lisenBluetoothStream() {
  char type;
  if(Serial.available() > 0) { 
    command = "";
    char ch = Serial.read();
        
    
    for(int j=0; j<recognice(ch); j++){
      char incomingByte = Serial.read();
      if(j==0)
        type = incomingByte;
      else
        command += incomingByte;
    }
  }
    
    int length_ = command.length();
    char temp[length_];
    command.toCharArray(temp, length_+1);
    

    if(type == MESSAGE){
      isMessage = true;
      displayOnLCD(temp);  
    }

    if(type == WAY) {
      isWay = true;
      letsGo(command);
    }

    if(type == END) {
      isEnd = true;
      isEnd = false;
    }
}

void displayOnLCD(char *ms){
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(ms);
  isMessage = false;
}

void sendToAndroid(char *ms, int i){
  
    for(int j=0; j<i; j++)
      Serial.print(ms[j]);
  
}

void letsGo(String command) {
  for(int i = 0; i < command.length(); i++){
    switch(command.charAt(i)) {
      case ONE_METER: 
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        digitalWrite(in3, HIGH);
        digitalWrite(in4, LOW);
        analogWrite(enA, 50);
        analogWrite(enB, 50);
        delay(3000);
        break;
      case TURN45: 
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        digitalWrite(in3, LOW);
        digitalWrite(in4, LOW);
        analogWrite(enB, 100);
        delay(1000);
        break;
      case TURN90: 
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        digitalWrite(in3, LOW);
        digitalWrite(in4, LOW);
        analogWrite(enB, 100);
        delay(2000);
        break;
      case TURN_BACK90: 
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        digitalWrite(in3, LOW);
        digitalWrite(in4, LOW);
        analogWrite(enB, 100);
        delay(2000);
        break;
     case TURN_BACK45: 
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        digitalWrite(in3, LOW);
        digitalWrite(in4, LOW);
        analogWrite(enB, 100);
        delay(1000);
        break;
    }

    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);

    isWay = false;  
  }
}

void addSecond() {
  seconds++;
}

