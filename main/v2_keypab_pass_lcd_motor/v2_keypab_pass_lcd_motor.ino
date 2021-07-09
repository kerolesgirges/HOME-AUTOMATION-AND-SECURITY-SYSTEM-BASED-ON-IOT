/*
  connection
  ground from node mcu connected to ground to ardiuno ( should be the same)
  ___________lcd-i2c______________
  lcd----->ardiuno
  gnd--->gnd
  vcc-->5v
  sda-->sda
  scl-->scl
  __________keybad____________
  keybad  ------->ardiuno
  columes 0:2 --->3:5
  rows    0:3---->6:9
  __________solinoid door lock _____________
  bin 2
  __________connecting-nodemcu___________
  ardiuno-------nodemcu
  A4  --------->D1
  A5  --------->D2
  ________buzzer______________
  bin 10
  ________magnitic_door_sensor______
  pin 11
  --------finger_print--------------
  
  3.3v -----> 5v 
  tx ------->rx 
  rx-------->tx
  gng------->gnd
*/
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include<EEPROM.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>

// configuration for keypad
const int ROW_NUM    = 4;    //four rows
const int COLUMN_NUM = 3;    //three columns
const int door_lock  = 2;
const int buzzer     = 10;
const int mag_sens   = 12;
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte pin_rows[ROW_NUM] = {9, 8, 7, 6};   //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {5, 4, 3}; //connect to the column pinouts of the keypad


Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );
const String password = "1234";          // change your password here
String input_password;
int pass_count = 0;
String strText;
int master_enter ;

//configuration for lcd
int lcdColumns = 16;
int lcdRows    = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
int cursorColumn = 11;
/**-*******************************************/
/*fingerpint config*/

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial);
uint8_t id;
//global status id
int door_stat = 0;

void setup() {
  Serial.begin(9600);
  pinMode(door_lock, OUTPUT);
  pinMode(mag_sens, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  digitalWrite(door_lock, LOW );
  digitalWrite(buzzer, LOW );
  //************************EEPROM INIT*****************************
  Serial.println("Reading from EEPROM....");
  delay(2000);
  for (int i = 0; i < 4; i++)
  {
    strText = strText + char(EEPROM.read(0x0F + i)); //Read one by one with starting address of 0x0F
  }
  input_password.reserve(32);           // maximum input characters is 33, change if needed
  //--------------------------i2c init

  Wire.begin(8);                        /* join i2c bus with address 8 */
  Wire.onReceive(receiveEvent);
  //Registers a function to be called when a slave device receives a transmission from a master.
  //Wire.onRequest(requestEvent);
  /* Register a function to be called when a master requests data from this slave device. */

  Serial.begin(9600);
  // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("enter pass:");
  lcd.setCursor(0, 1);
  lcd.print("( * )to clear");
  lcd.setCursor(cursorColumn, 0);

  // fingerprint init

  Serial.begin(57600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) {
      delay(1);
    }
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains ");
  Serial.print(finger.templateCount);
  Serial.println(" templates");
  Serial.println("Waiting for valid finger...");


}

// door_stat=1 rfid correct
// door_stat=2 rfid not correct
// door_stat=3 finger_is_correct
// door_stat=4 finger_is'n_correct
// door_stat=5 door_is_closed from magnitic sensor
void loop() {
  Serial.print(millis());
  Serial.print("\n");
  char key = keypad.getKey();
  getFingerprintIDez();
   
  if (digitalRead(mag_sens) == 0 ) {  // if door  closed 
    //delay(2000);                  
    digitalWrite(door_lock, LOW);
    lcd.setCursor(0, 0);
    lcd.print("enter pass:");
    lcd.setCursor(0, 1);
    lcd.print("( * )to clear");
    //lcd.setCursor(cursorColumn, 0);
    //door_closed();
  
 }
  if (door_stat == 1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("rfid correct");
    delay(1000);
    door_open();
    door_stat = 0;

  }
  else if (door_stat == 2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("rfid not correct");
    delay(1000);
    door_closed();
    door_stat = 0;
  }

  else if (door_stat == 3) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("finger correct ");
    delay(1000);
  /*******************/
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("door open");
  digitalWrite(door_lock, HIGH);
  delay(750);
  digitalWrite(buzzer, HIGH );
  delay(1000);
  digitalWrite(buzzer, LOW );
  delay(5000);
  if(digitalRead(mag_sens) == 0){ // if closed
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("door closed");
  digitalWrite(door_lock, LOW);
  delay(750);
  input_password = "";       // clear input password

  }

  /******************/
    door_stat = 0;
  }
   
  // else if (door_stat == 0) {
  // if (digitalRead(mag_sens) == 0 ) {
   // delay(2000);
   // digitalWrite(door_lock, LOW);
    //door_closed();
  //}
  //}
  
  
  /*
      else if (door_stat == 5) {
     door_closed();
     door_stat = 0;
    }
  */
  if (key) {
    Serial.println(key);
    input_password += key;          // append new character to input password string
    lcd.setCursor(cursorColumn, 0); // move cursor to   (cursorColumn, 0)
    digitalWrite(buzzer, HIGH );
    delay(500);
    digitalWrite(buzzer, LOW );
    if (key != '*')
    {
      lcd.print("*");                // print key at (cursorColumn, 0)
    }
    cursorColumn++;
    Serial.println("input_password.length :");
    Serial.println(input_password.length());
    Serial.println("");
    delay(1000);
    if (key == '*')
    {
      input_password = "";           // clear input password
      door_closed();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("enter pass:");
      lcd.setCursor(0, 1);
      lcd.print("( * )to clear");
      cursorColumn = 11;

    }
    else if ( input_password.length() == 4)   //enter key
    {

      if (input_password == strText)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("correct pass");
        delay(1000);
        Serial.println("password is correct");
        Serial.println(input_password.length());
        door_stat = 0;
        door_open();
        //        //**********************************DO YOUR WORK HERE********************
        //        lcd.clear();
        //        lcd.setCursor(0, 0);
        //        lcd.print("door is  open..");
        //        //***********************************************************************
      }
      else
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("incorrect pass");
        delay(1000);
        door_closed();
        //        lcd.clear();
        //        lcd.setCursor(0, 0);
        //        lcd.print("enter pass:");
        //        lcd.setCursor(0, 1);
        //        lcd.print("* clear ");

        cursorColumn = 11;
        Serial.println("password is incorrect, try again");
        pass_count = 0;
      }

      input_password = "";       // clear input password
    }
    else
    {

      //input_password += key;   // append new character to input password string

    }
  }
}
// function that executes whenever data is received from master

void receiveEvent() {
  char c ;

  while (0 < Wire.available()) {
    c = Wire.read();            /* receive byte as a character */
    Serial.print("i2c receved=");
    delay(500);
    Serial.print(c);            /* print the character */
    //Serial.print("done!!!");
    door_stat = (int)(c);
    door_stat = door_stat - 48;
    Serial.print(door_stat);
  }

  Serial.println();             /* to newline */
}
void door_open() {
  //**********************************DO YOUR WORK HERE********************
   delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("door open");
  digitalWrite(door_lock, HIGH);
  delay(750);
  digitalWrite(buzzer, HIGH );
  delay(1000);
  digitalWrite(buzzer, LOW );
  delay(5000);
  //***********************************************************************
}
void door_closed() {
  if (digitalRead(mag_sens) == 0 ) {
    digitalWrite(buzzer, HIGH );
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("door is closed..");
    Serial.print("closed");
    digitalWrite(door_lock, LOW);
    delay(1000);
    digitalWrite(buzzer, LOW );
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("enter pass:");
    lcd.setCursor(0, 1);
    lcd.print("( * )to clear");
    cursorColumn = 11;
  }
}

int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);

  if (finger.fingerID == 1 && finger.confidence >= 40) {
    Serial.print("\n hi koko ");
    door_stat = 3;
    //door_open();

  }
  if (finger.fingerID == 2 && finger.confidence >= 40) {
     Serial.print("\n hi koko ");
    door_stat = 3;
  }

  return finger.fingerID;
}
