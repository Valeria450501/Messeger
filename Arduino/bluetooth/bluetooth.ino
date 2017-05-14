#include <Thread.h> 
#include <LiquidCrystal.h>
#include <TimerOne.h>
#include <SoftwareSerial.h>

#define START '0'
#define MESSAGE '1'
#define WAY '2'
#define END '3'

#define ONE_METER '0'
#define TURN45 '1'
#define TURN90 '2'
#define TURN_BACK45 '3'
#define TURN_BACK90 '4'

struct mission {
  String message;
  String way;
  byte currentStep = 0;
  bool isStart = false;
  bool isMessage = false;
  bool isWay = false;
  bool isEnd = false;
} MISSION;

struct state {
  bool START_MISSION = false;
  bool CONNECTION_ESTABLISHED = false;
  bool MISSION_EXECUTION = false;
  bool READ_COMMAND = false;
  bool DELIVERED_MESSAGE = false;
  bool NOT_AVAILABLE_MOTION = false;
} MAX;

Thread driveThread = Thread(); 
Thread lcdThread = Thread();
Thread bluetoothListenerThread = Thread();
LiquidCrystal lcd(8, 9, 10, 11, 12, 13); // RS E D4 D5 D6 D7
SoftwareSerial BTserial(2, 3); // RX | TX

byte tempState = 0;

// первый двигатель
int enA = A0;
int in1 = A1;
int in2 = A2;

// второй двигатель
int enB = A5;
int in3 = A3;
int in4 = A4;

int echoPin = 4; 
int trigPin = 5; 

void setup() {
  Serial.begin(9600);
  BTserial.begin(9600); 
  lcd.begin(16, 2);

  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);
  
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  
  int repeat = 0;
  while(!MAX.CONNECTION_ESTABLISHED){
    if(repeat == 0)
      displayOnLCD("Hi, I'm Max!");
    if(repeat == 10)
      displayOnLCD("Ready to start!");
    if(repeat == 20)
      displayOnLCD("I am the slave of this lamp!");
    if(repeat == 30)
      repeat = -1;

    repeat++;  
    setContact();
    delay(1000);
  }

  displayOnLCD("--Connected!--");
  delay(2000);

  bluetoothListenerThread.onRun(listenBluetoothStream);
  bluetoothListenerThread.setInterval(20);

  lcdThread.onRun(listenLCDStream);
  lcdThread.setInterval(2000);
  /*Timer1.initialize();
  Timer1.attachInterrupt(addSecond);*/

  
 driveThread.onRun(letsGo);
 driveThread.setInterval(4000);
  
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

int recogniceCharInt(char ch){
  switch(ch) {
    case '1':
      return 1;
    case '2':
      return 2;
    case '3':
      return 3;
    case '4':
      return 4;
    case '5':
      return 5;
    case '6':
      return 6;
    case '7': 
      return 7;
    case '8':
      return 8;
    case '9':
      return 9;
    case '0':
      return 0;
    default:
      return 10;
  }
}

bool checkIsStart(){
  if(MISSION.isWay) {
    Serial.println("Max: Error: Way already selected.");
    return false;
  }
  if(MISSION.isMessage){
    Serial.println("Max: Error: Message already selected.");
    return false;
  }
  if(MISSION.isStart){
    Serial.println("Max: Error: Start is already selected.");
    return false;
  }
  if(MAX.MISSION_EXECUTION) {
    Serial.println("Max: Error: Mission is already in progress.");
    return false;
  }

  return true;
}

bool checkIsMessage(){
  if(MISSION.isMessage){
    Serial.println("Max: Error: Message already selected.");
    return false;
  }
  if(!MISSION.isStart){
    Serial.println("Max: Error: Mission is not started.");
    return false;
  }
  if(MAX.MISSION_EXECUTION) {
    Serial.println("Max: Error: Mission is already in progress.");
    return false;
  }

  return true;
}

bool checkIsWay(){
  if(MISSION.isWay){
    Serial.println("Max: Error: Way already selected.");
    return false;
  }
  if(!MISSION.isStart){
    Serial.println("Max: Error: Mission is not started.");
    return false;
  }
  if(MAX.MISSION_EXECUTION) {
    Serial.println("Max: Error: Mission is already in progress.");
    return false;
  }

  return true;
}

bool checkIsEnd(){
  if(!MISSION.isStart){
    Serial.println("Max: Error: Start not selected.");
    return false;
  }
  if(!MISSION.isWay) {
    Serial.println("Max: Error: Way not selected.");
    return false;
  }
  if(!MISSION.isMessage){
    Serial.println("Max: Error: Message not selected.");
    return false;
  }

  return true;
}

void recogniceCommand(String command){
  switch(command.charAt(0)){
    case START:
      if(checkIsStart()) {
        MISSION.message = "";
        MISSION.way = "";
        MISSION.isStart = true;
        MAX.START_MISSION = true;
        Serial.println("Max: Mission start.");
      }
      break;
    case MESSAGE:
      if(checkIsMessage()) {
        MISSION.message += command.substring(1);
        if(command.charAt(command.length() - 1) == '$'){
          MISSION.isMessage = true;
          Serial.println("Max: mes: " + command + " . $");
        } else {
          Serial.println("Max: mes:" + command + " ->");
        }
      }
      break;
    case WAY:
      if(checkIsWay()) {
        MISSION.way += command.substring(1);
        if(command.charAt(command.length() - 1) == '$'){
          MISSION.isWay = true;
          Serial.println("Max: way:" + command.substring(1) + " . $");
        } else {
          Serial.println("Max: way:" + command.substring(1) + " ->");
        }
      }
      break;
    case END:
      if(checkIsEnd()) {
        MISSION.isStart = false;
        MISSION.isMessage = false;
        MISSION.isWay = false;
        MISSION.isEnd = true;
        MAX.START_MISSION = false;
        Serial.println("Max: Command end.");
      }
      break;
    default:
      displayOnLCD("Command error");
  }

}

void loop() {
  if (driveThread.shouldRun())
    driveThread.run();
          
  if (bluetoothListenerThread.shouldRun() )
    bluetoothListenerThread.run();

  if (lcdThread.shouldRun() )
    lcdThread.run();
}

void listenLCDStream() {
  if(MAX.MISSION_EXECUTION) {
    if(MAX.NOT_AVAILABLE_MOTION) {
      displayOnLCD("HEDGE");
      return;  
    }
    
   displayOnLCD(" --Diverged!-- ");
   return;
  }
  
  if(MAX.READ_COMMAND) {
   displayOnLCD("Read command");
   return;
  }
  if(MAX.START_MISSION) {
   displayOnLCD("Data download.");
   return;
  }

  if(MAX.CONNECTION_ESTABLISHED) {
   displayOnLCD("Let's start!");
   return;
  }
}

void listenBluetoothStream() {
  char type;
  String command = "";
  
  if(BTserial.available() > 0) {
    MAX.READ_COMMAND = true;
    char ch = BTserial.read();
    Serial.print("Max: read from listenBluetoothStream length -> "); Serial.println(ch);

    int mes_length = recogniceCharInt(ch);
    if(mes_length < 10 && mes_length > 0) {
      for(int j=0; j<mes_length; j++){
        ch = BTserial.read();
        command +=  ch;
      }    
      
      Serial.println("  command ->"); Serial.println("  ->  " + command);
  
      recogniceCommand(command);
    } else {
      displayOnLCD("Command error");
    }

    MAX.READ_COMMAND = false;
  }   
}

void displayOnLCD(String mes){
  int length_ = mes.length();
  char temp[length_];

  if(length_ > 15) {
    char current[15];
    byte offset = 0;
     
    for (byte k = 0 ; k < length_ - 15 ; k++) {
      for (byte i = 0 ; i <= 15 ; i++) { 
        current[i] = mes.charAt(i + offset);
      }
      offset ++;
      delay(500); 
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print(current); 
    }

    for (byte i = 0 ; i <= 15 ; i++) {
      current[i] = mes.charAt(i);
    }
    Serial.print("Max: display on lsd: "); Serial.println(current);
  } else {
    mes.toCharArray(temp, length_+1);
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(temp);
    Serial.print("Max: display on lsd: "); Serial.println(temp);
  }  
}

void letsGo() {
  if(MISSION.isEnd) {
    MAX.MISSION_EXECUTION = true;
    displayOnLCD(" --Diverged!-- ");

    if(MISSION.currentStep == MISSION.way.length() - 1) {
      MAX.DELIVERED_MESSAGE = true;
      Serial.println("Max: I'm delivered message.");
    }

    if(MISSION.currentStep == 255 && MAX.DELIVERED_MESSAGE) {
      Serial.println("Max: I'm returned.");
      MAX.MISSION_EXECUTION = false;
      MAX.DELIVERED_MESSAGE = false;
      MISSION.isEnd = false;
      return;
    }
          
    if(getDistance() < 50) {
      delay(1000);

      MAX.NOT_AVAILABLE_MOTION = true;
      Serial.println("Max: Barrier on my way!");  
      displayOnLCD("Stop");
      return;
    }

    MAX.NOT_AVAILABLE_MOTION = false;
    
    switch(MISSION.way.charAt(MISSION.currentStep)) {
      case ONE_METER:
        Serial.println("Max: One meter.");         
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        digitalWrite(in3, LOW);
        digitalWrite(in4, HIGH);
        analogWrite(enA, 50);
        analogWrite(enB, 50);
        delay(4000);
        break;
      case TURN45:
        Serial.println("Max: Turn 45."); 
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        digitalWrite(in3, LOW);
        digitalWrite(in4, LOW);
        analogWrite(enB, 100);
        delay(2000);
        break;
      case TURN90:
        Serial.println("Max: Turn 90.");
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        digitalWrite(in3, LOW);
        digitalWrite(in4, LOW);
        analogWrite(enB, 100);
        delay(4000);
        break;
      case TURN_BACK90:
        Serial.println("Max: Turn back 90."); 
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        digitalWrite(in3, LOW);
        digitalWrite(in4, LOW);
        analogWrite(enB, 100);
        delay(4000);
        break;
     case TURN_BACK45:
        Serial.println("Max: Turn back 45."); 
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        digitalWrite(in3, LOW);
        digitalWrite(in4, LOW);
        analogWrite(enB, 100);
        delay(2000);
        break;
    }

    Serial.print("Max: step: -"); Serial.println(MISSION.currentStep);  
    if(!MAX.DELIVERED_MESSAGE) {
      MISSION.currentStep++;
    } else {
      MISSION.currentStep--;
    }
    
    stopMotion();
  }
}

bool setContact() {
  char ch;
  MAX.CONNECTION_ESTABLISHED = false;
  Serial.println("... set connection ...");
 
  if(BTserial.available() > 0) { 
    ch = BTserial.read();

    Serial.print("Max read:");Serial.println(ch); 
    
    if(ch != '@')
      return false;
      
  } else {
    return false;
  }

  BTserial.print('!');

  Serial.println("Max write:@~");
  int k=0;
  
  while(k < 1000000) {
    if(BTserial.available() > 0) { 
      ch = BTserial.read();

      Serial.print("Max read:");Serial.println(ch); 
      
      if(ch == '~'){
        Serial.println("Max: Connection established!");
        MAX.CONNECTION_ESTABLISHED = true;
        return true;
      }
    } 
    
    k++;
  }

  return false;
}


int getDistance() {
    int duration, cm; 
    digitalWrite(trigPin, LOW); 
    delayMicroseconds(10); 
    digitalWrite(trigPin, HIGH); 
    delayMicroseconds(10); 
    digitalWrite(trigPin, LOW); 
    duration = pulseIn(echoPin, HIGH); 
    cm = duration / 58;
    
    Serial.print("Max: get distance: ");
    Serial.println(cm); 

    return cm;
}

void stopMotion(){
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW);
}

