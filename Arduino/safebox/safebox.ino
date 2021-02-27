#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Servo.h>

//objects (lcd, servo, keypad, buzzer)

//LiquidCrystal(rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(13, 12, 5, 4, 3, 2);


const byte rows = 4; //four rows
const byte cols = 4; //four columns
char keys[rows][cols] = {
  {'7','8','9','/'},
  {'4','5','6', '*'},
  {'1','2','3', '-'},
  {'c', '0', '=', '+'}
};
byte rowPins[rows] = {22, 23, 24, 25}; //connect to the row pinouts of the keypad
byte colPins[cols] = {26, 27, 28, 29}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );

//servo
Servo myservo;

const int buzPin = 0;
const int green = 7;
const int red = 1;

//variables
int password = 1377;
long entered_pass;
boolean finished;
boolean box_is_open;
boolean counting;
int counter = 9;

//functions

boolean checkPass(int pass){
  if(password == pass)return true;
  else return false;  
}

int toInteger(char c) {
    return c - '0';
}

void reset(){
      lcd.clear();
      entered_pass = 0;
      finished = false; 
      box_is_open = false;
      counter = 10;
      counting = false;
}

void turnOnLED(boolean ul){
    if(ul){
      digitalWrite(green, HIGH);
      digitalWrite(red, LOW);
      }
    else {
      digitalWrite(red, HIGH);
      digitalWrite(green, LOW);
    }
}

void unlock(){
    myservo.write(150);
    tone(buzPin, 200);
    delay(300);
    noTone(buzPin);
}

void lock(){
    myservo.write(50);
    tone(buzPin, 200);
    delay(300);
    noTone(buzPin);
  }

//the first info showing on the screen
void displayLCD(){
    lcd.clear();
    lcd.setCursor(0, 0);
    if(checkPass(entered_pass)) lcd.print("Correct PSW");
    else lcd.print("Wrong PSW/LOCKED");
    lcd.setCursor(0, 1);
    lcd.print(entered_pass);
    delay(500);
}
//display the state of the door
void displayLCD_info(){
    lcd.clear();
    lcd.setCursor(0, 0);
    if(box_is_open) lcd.print("UNLOCKED");
    else lcd.print("LOCKED");
    lcd.setCursor(0, 1);
    delay(500);
}

void changePass(){
    int newPass = 0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter new PSW");
    lcd.setCursor(0, 1);
    while(1){
       char key = keypad.getKey();
      //receiving the pass
      if(key >= '0' && key <= '9' && key != '='){
        newPass = newPass*10 + toInteger(key);
        lcd.print(key);
      }else if(key == '=') {
              password = newPass;
              lcd.setCursor(0, 1);
              lcd.print("PSW changed");
              delay(500);
              lcd.clear();
              break;
          }
//            else{
//              lcd.setCursor(0, 1);
//              lcd.print("Invalid input");
//              delay(200);
//              lcd.clear();
//              return;
//        } 
    }
}

void changeCounter(){
    int newcounter = 0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("New counter");
    lcd.setCursor(0, 1);
    while(1){
       char key = keypad.getKey();
      //receiving the pass
      if(key >= '0' && key <= '9' && key != '='){
        newcounter = newcounter*10 + toInteger(key);
        lcd.print(key);
      }else if(key == '=') {
              counter = newcounter;
              lcd.setCursor(0, 1);
              lcd.print("counter changed");
              delay(500);
              lcd.clear();
              break;
          }
  }
}

void setup() {
   //LCD
   lcd.begin(16,2);
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("Shagha Safe box");
   lcd.setCursor(0, 1);
   lcd.print("Enter PSW");
   delay(500);
   lcd.clear();
   myservo.write(50);

  //buzeer
   pinMode(buzPin,OUTPUT);
   //LED
   pinMode(red, OUTPUT);
   pinMode(green, OUTPUT);
   
   //servo
   myservo.attach(6);

   //initilization 
   entered_pass = 0;
   finished = false;
   box_is_open = false;
   counting = false;
   
}

void loop() {
  //turn on the LED base on the safebox's state
  turnOnLED(finished && box_is_open);
  //scan the entered password
  char key = keypad.getKey();
  if(key){
    //receiving the pass
    if(key >= '0' && key <= '9'){
      entered_pass = entered_pass*10 + toInteger(key);
      lcd.print(key);
    }else{
      //entering is finished
      if(key == '+') finished = true;
      else{
          //closing safebox
          if(key == '-'){
                unlock();
                reset();
            } 
          else {if(key == '/'){
                changePass();
            }
            else{
                if(key == '*'){
                    changeCounter(); 
                }
                else if(key == 'c') reset();
              }
          } 
      } 
    }  
  }
  if(finished && !box_is_open) {
    displayLCD();
    if(!checkPass(entered_pass)){
        key = keypad.getKey();
        while(!key){
            key = keypad.getKey();
            if(key == 'c') reset();
        }
    } 
  }
  if(finished && checkPass(entered_pass) && !box_is_open){
      unlock();
      box_is_open = true;
      counting = true;
      turnOnLED(true);
  }
  if(finished && checkPass(entered_pass)){
       displayLCD_info(); 
  }
  if(counting){
    lcd.setCursor(0, 1);
    lcd.print("timer ");
    int timer = counter;
    for(int i = 0; i < counter; i++){
        key = keypad.getKey();
        if(!key){
            lcd.setCursor(7, 1);
            lcd.print(timer);
            timer--;
            delay(500);
        }
        else break; 
    }
    lock();
    reset();
     
  }    
   

}
