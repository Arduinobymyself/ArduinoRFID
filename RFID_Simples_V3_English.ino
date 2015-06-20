/*
#####################################################################################
#  	File:               RFID_Simples_V3.ino                                             
#       micro controller:   Arduino UNO ou Teensy 2.0++      
#  	Language:	    Wiring / C /Processing /Fritzing / Arduino IDE
#       Version:            1.0
#						
#	Objectives:         Arduino RFID - Security System and Access Control
#										  
#	Operation:	    Using the RFID MFCR-522AN reader, we can do the personal access control
#                           to an specific environment.
#                           To do this we need to check the card or tag ID
#                           If a valid ID is found it is possible to lit a LED, play a soundand drive a servo-motor
#                           to open the door or gate - if an invalid ID is found, it is possible to do the access rejection
#                           flashing a red LED and playing an alert sound and locking the door.
#                           A LCD (20 columm x 4 rows shos us the system messages
#
#			
#   Author:              Marcelo Moraes 
#   Date:                06/16/15	
#   Place:               Sorocaba - SP, Brazil
#					
#####################################################################################
 
This project contains public domain code. 
The modification is permitted without notice.


 ******PINS FOR ARDUINO*********
 *** Reset = Pin 5 ***
 *** SDA = Pin 10 ***
 *** MOSI = Pin 11 ***
 *** MISO = Pin 12 ***
 *** SCK = Pin 13 ***
 *** GND = GROUND ***
 *** 3.3 = 3.3v ***
 
 ******************************************
 
*/


// libraries
#include <SPI.h>
#include <RFID.h>
#include <Servo.h>
#include "pitches.h"
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// RFID definition
RFID rfid(10,5);

// 5 data bytes from cards
// you can read these bytes on the Serial console
byte emaomos[5] = {0xDE,0x1F,0x47,0xC9,0x4F};  // Marcelo Moraes (e+mao+mos)
byte newuser1[5] = {0xB5,0x3B,0x14,0xB7,0x2D}; //new User 1 Flora
byte newuser2[5] = {0x01,0x9A,0x92,0x2b,0x23}; //new User 2
// Add allowed card IDs here

// // LCD address and type declaration
LiquidCrystal_I2C lcd(0x27,20,4);

byte serNum[5];
byte data[5];
int cardRead;  // card read 1 = good 0 = bad for playTune function
String Name;   // used for user name displayMsg function

// Melodies definition: access, welcome and rejection
int access_melody[] = {NOTE_G4,0, NOTE_A4,0, NOTE_B4,0, NOTE_A4,0, NOTE_B4,0, NOTE_C5,0};
int access_noteDurations[] = {8,8,8,8,8,4,8,8,8,8,8,4};
int fail_melody[] = {NOTE_G2,0, NOTE_F2,0, NOTE_D2,0};
int fail_noteDurations[] = {8,8,8,8,8,4};

// pins definition - LED, Buzzer and Servo-motor
int LED_access = 3;
int LED_intruder = 4;
int speaker_pin = 8;
int servoPin = 6;

// servo motor definition
Servo doorLock;


void setup(){
  doorLock.attach(servoPin);    // servo motor attaching
  Serial.begin(9600);           // Serial communication initialization
  lcd.init();                   // LCD initialization
  lcd.backlight();
  lcd.clear();                  // Clears the LCD display
  SPI.begin();                  // SPI communication initialization
  rfid.init();                  // RFID module initialization
  Serial.println("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
  Serial.println("+ http://arduinobymyself.blogspot.com.br                                              +"); 
  Serial.println("+ Arduino RFID Security System using MFRC522-AN                                       +");
  Serial.println("+ The RFID module is initiate in the automatic read mode, waiting for a valid card... +");
  Serial.println("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
  displayMsgInitial();
  delay(3000);
  pinMode(LED_access,OUTPUT);
  pinMode(LED_intruder,OUTPUT);
  pinMode(speaker_pin,OUTPUT);
  pinMode(servoPin,OUTPUT);
}
 
void loop(){
  lcd.clear();
  lcd.noBacklight();
  // Here you can create a variable for each user
  // NAME_card ou KEY_card
  boolean emaomos_card = true; // that is my card (e + mao + mos)...e + marcelo = mao, moraes = mos
  boolean newuser1_card = true;
  boolean newuser2_card = true;
  //put another users here
  
  if (rfid.isCard()){ // valid card found
    if (rfid.readCardSerial()){ // reads the card
      delay(1000);
      data[0] = rfid.serNum[0]; // stores the serial number
      data[1] = rfid.serNum[1];
      data[2] = rfid.serNum[2];
      data[3] = rfid.serNum[3];
      data[4] = rfid.serNum[4];
    }
  rfid.halt(); // RFID module to standby
  cardRead = 0;
  displayID_LCD();
  displayID_Console(); // used for testing purposes only. Prints the Card ID at the serial console 
  for(int i=0; i<5; i++){
    if(data[i] != emaomos[i]) emaomos_card = false; // if it is not a valid card, put false to this user
    if (data[i] != newuser1[i]) newuser1_card = false;
    if (data[i] != newuser2[i]) newuser2_card = false;
    // Here you can check the another allowed users, just put lines like above with the user name
  }
  
  Serial.println(); // used for testing purposes  only
  if (emaomos_card){ // if a valid card was found
    cardRead = 1;
    displayMsgOK("Marcelo");
    playTune(cardRead);
  }
  else if(newuser1_card){
    cardRead = 1;
    displayMsgOK("Sara");
    playTune(cardRead);
  }
  else if(newuser2_card){
    cardRead = 1;
    displayMsgOK("newuser2");
    playTune(cardRead);
  }
  // another cards analysis, put many blocks like this 
  // as many users you have
  
  else{ // if a card is not recognized
    displayMsgNOK();
    Serial.println("Card not recognized!  contact administration!"); // used for testing purposes  only
    digitalWrite(LED_intruder, HIGH); // turn on the red LED
    playTune(cardRead);
    delay(1000);
    digitalWrite(LED_intruder, LOW); // turn off the red LED
  }
  if (emaomos_card || newuser1_card || newuser2_card){// add another user using an logical or condition || 
    Serial.println("Access Granted!... Welcome to ArduinoByMySelf!"); // used for testing purposes  only
    digitalWrite(LED_access,HIGH); // turn on the green LED
    doorLock.write(0); // releases the door, you need to adjust this to positioning the servo according your door locker
    delay(3000); // wait 5 senconds
    doorLock.write(180); // Locks the door, brings the serve to the original position should be adjusted too....
    digitalWrite(LED_access,LOW); // turn off the green LED
  }
  Serial.println(); // used for testing purposes  only
  delay(1000);
  rfid.halt(); // wait till sense a card over the reader
  lcd.noBacklight(); // turn the LCD backlight off
  }
}





//========== Function to play the access granted or denied tunes ==========
void playTune(int Scan) {    
  if (Scan == 1) // A Good card Read
  { 
    for (int i = 0; i < 12; i++)    //loop through the notes
        { // Good card read
          int access_noteDuration = 1000 / access_noteDurations[i];
          tone(speaker_pin, access_melody[i], access_noteDuration);
          int access_pauseBetweenNotes = access_noteDuration * 1.30;
          delay(access_pauseBetweenNotes);
          noTone(speaker_pin);
       }
  }     
    else // A Bad card read
       for (int i = 0; i < 6; i++)    //loop through the notes 
       { 
          int fail_noteDuration = 1000 / fail_noteDurations[i];
          tone(speaker_pin, fail_melody[i], fail_noteDuration);
          int fail_pauseBetweenNotes = fail_noteDuration * 1.30;
          delay(fail_pauseBetweenNotes);
          noTone(speaker_pin);
       }
  }
  
  
//========== Display LCD messages for the users ========== 
void displayMsgOK(String user)  
{
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("  Welcome ");
    lcd.print(user);
    lcd.setCursor(0,3);
    lcd.print("--------------------");
    lcd.setCursor(0,2);
    lcd.print("   Access Granted   ");
    lcd.setCursor(0,3);
    lcd.print("      Welcome!      ");
    
} 

void displayMsgNOK()  
{
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("********************");
    lcd.setCursor(0,1);
    lcd.print("*  Contact Admin!  *");
    lcd.setCursor(0,2);
    lcd.print("*  Access denied!  *");
    lcd.setCursor(0,3);
    lcd.print("********************");
}

void displayMsgInitial()  
{
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("********************");
    lcd.setCursor(0,1);
    lcd.print("*  ArduinoByMyself *");
    lcd.setCursor(0,2);
    lcd.print("*  Security System *");
    lcd.setCursor(0,3);
    lcd.print("********************");
}

void displayID_LCD(){
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("********************");
  lcd.setCursor(0,1);
  lcd.print("*     ID Found     *");
  //Serial.print("Card found - code:");
  lcd.setCursor(0,2);
  lcd.print("*    ");
  if(rfid.serNum[0] < 16){
    lcd.print("0");
  }
  lcd.print(rfid.serNum[0],HEX);
  
  if(rfid.serNum[1] < 16){
    lcd.print("0");
  }
  lcd.print(rfid.serNum[1],HEX);
  
  if(rfid.serNum[2] < 16){
    lcd.print("0");
  }
  lcd.print(rfid.serNum[2],HEX);
  
  if(rfid.serNum[3] < 16){
    lcd.print("0");
  }
  lcd.print(rfid.serNum[3],HEX);
  
  if(rfid.serNum[4] < 16){
    lcd.print("0");
  }
  lcd.print(rfid.serNum[4],HEX);
  
  lcd.print("    *");
  
  lcd.setCursor(0,3);
  lcd.print("********************");
  delay(1000); // shows the ID for about 1000ms
  lcd.clear();
  lcd.noBacklight();
}



void displayID_Console(){
  Serial.print("Card found - code:");
  if(rfid.serNum[0] < 16){
    Serial.print("0");
  }
  Serial.print(rfid.serNum[0],HEX);
  
  if(rfid.serNum[1] < 16){
    Serial.print("0");
  }
  Serial.print(rfid.serNum[1],HEX);
  
  if(rfid.serNum[2] < 16){
    Serial.print("0");
  }
  Serial.print(rfid.serNum[2],HEX);
  
  if(rfid.serNum[3] < 16){
    Serial.print("0");
  }
  Serial.print(rfid.serNum[3],HEX);
  
  if(rfid.serNum[4] < 16){
    Serial.print("0");
  }
  Serial.print(rfid.serNum[4],HEX);
}

