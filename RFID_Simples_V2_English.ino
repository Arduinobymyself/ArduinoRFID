/*
#####################################################################################
#  	File:               RFID_Simples.ino                                             
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
#   Date:                27/05/13	
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

byte emaomos[5] = {0xDE,0x1F,0x47,0xC9,0x4F}; // Marcelo Moraes (e+mao+mos)
// Put here the another allowed cards

// // LCD address and type declaration
LiquidCrystal_I2C lcd(0x27,20,4);

byte serNum[5];
byte data[5];

// Melodies definition: access, welcome and rejection
int access_melody[] = {NOTE_G4,0,NOTE_A4,0, NOTE_B4,0,NOTE_A4,0,NOTE_B4,0, NOTE_C5,0};
int access_noteDurations[] = {8,8,8,8,8,4,8,8,8,8,8,4};
int fail_melody[] = {NOTE_G2,0,NOTE_F2,0,NOTE_D2,0};
int fail_noteDurations[] = {8,8,8,8,8,4};

// pins definition - LED, Buzzer and Servo-motor
int LED_access = 3;
int LED_intruder = 4;
int speaker_pin = 8;
int servoPin = 6;

// servo motor definition
Servo doorLock;


void setup(){
  doorLock.attach(servoPin); // servo motor attaching
  Serial.begin(9600); // Serial communication initialization
  lcd.init(); // LCD initialization
  lcd.backlight();
  lcd.clear();// Clears the LCD display
  SPI.begin(); // SPI communication initialization
  rfid.init(); // RFID module initialization
  Serial.println("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
  Serial.println("+ http://arduinobymyself.blogspot.com.br                                              +"); 
  Serial.println("+ Arduino RFID Security System using MFRC522-AN                                       +");
  Serial.println("+ The RFID module is initiate in the automatic read mode, waiting for a valid card... +");
  Serial.println("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
  delay(1000);
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
  
  if (rfid.isCard()){ // valid card found
    if (rfid.readCardSerial()){ // reads the card
      delay(1000);
      data[0] = rfid.serNum[0]; // stores the serial number
      data[1] = rfid.serNum[1];
      data[2] = rfid.serNum[2];
      data[3] = rfid.serNum[3];
      data[4] = rfid.serNum[4];
    }
  
  //rfid.halt(); // RFID module to standby
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("   ID found:");
  //Serial.print("Card found - code:");
  lcd.setCursor(0,1);
  lcd.print("     ");
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
  
  for(int i=0; i<5; i++){
    if(data[i] != emaomos[i]) emaomos_card = false; // if it is not a valid card, put false to this user
    // Here you can check the another allowed users, just put lines like above with the user name
  }
  Serial.println();
  if (emaomos_card){ // if a valid card was found
    lcd.setCursor(0,2);
    lcd.print("Hello User Marcelo!");
    //Serial.println("Hello user Marcelo!"); // print a message
    for (int i = 0; i < 12; i++){ // play welcome sound
      int access_noteDuration = 1000/access_noteDurations[i];
      tone(speaker_pin, access_melody[i],access_noteDuration);
      int access_pauseBetweenNotes = access_noteDuration * 1.30;
      delay(access_pauseBetweenNotes);
      noTone(speaker_pin);
    }
  }
  /*
  // another cards analysis put many blocks like this as many user you have
  else if(USER_card){// put the another allowed user here
    lcd.setCursor(0,3);
    lcd.print("Hello USER NAME");
    //Serial.println("Hello USER NAME"); // put the user name in the "USER NAME"
    for (int i = 0; i < 12; i++){
      int access_noteDuration = 1000/access_noteDurations[i];
      tone(speaker_pin, access_melody[i],access_noteDuration);
      int access_pauseBetweenNotes = access_noteDuration * 1.30;
      delay(access_pauseBetweenNotes);
      noTone(speaker_pin);
    }
  }
  */
  else{ // if a card is not recognized
    lcd.setCursor(0,2);
    lcd.print(" ID not reconized!");
    lcd.setCursor(0,3);
    lcd.print("   Access denied!");
    //Serial.println("Card not recognized!  contact administration!"); // print a message
    digitalWrite(LED_intruder, HIGH); // turn on the red LED
    for (int i = 0; i < 6; i++){ // playa rejection sound
      int fail_noteDuration = 1000/fail_noteDurations[i];
      tone(speaker_pin, fail_melody[i],fail_noteDuration);
      int fail_pauseBetweenNotes = fail_noteDuration * 1.30;
      delay(fail_pauseBetweenNotes);
      noTone(speaker_pin);
    }
    delay(1000);
    digitalWrite(LED_intruder, LOW); // turn off the red LED
  }
  if (emaomos_card){// add another user using an logical or condition ||
    // Welcome messages and access permission  
    lcd.setCursor(0,2);
    lcd.print("  Access Granted!");
    lcd.setCursor(0,3);
    lcd.print("   Welcome!");
    //Serial.println("Access Granted!... Welcome to ArduinoByMySelf!"); // print a message
    digitalWrite(LED_access,HIGH); // turn on the green LED
    doorLock.write(180); // releases the door
    delay(5000); // wait
    doorLock.write(0); // Locks the door
    digitalWrite(LED_access,LOW); // turn off the green LED
  }
  Serial.println();
  delay(500);
  rfid.halt();
  lcd.noBacklight();
  }
}
  

