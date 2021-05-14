
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include<EEPROM.h>
#include <Wire.h>
//  configuration for keypad
const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 3; //three columns


char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte pin_rows[ROW_NUM] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {5, 4, 3}; //connect to the column pinouts of the keypad


Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );
const String password = "1234"; // change your password here
String input_password;
int pass_count = 0;
String strText;
int master_enter ;

//configuration for lcd
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
int cursorColumn = 11;

int x = 0;
void setup() {
  Serial.begin(9600);
  Serial.println("Reading from EEPROM....");
  delay(2000);
  for (int i = 0; i < 4; i++)
  {
    strText = strText + char(EEPROM.read(0x0F + i)); //Read one by one with starting address of 0x0F
  }
  input_password.reserve(32); // maximum input characters is 33, change if needed
  //i2c init

  Wire.begin(8);                /* join i2c bus with address 8 */
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
  lcd.print("* clear ");
  lcd.setCursor(cursorColumn, 0);
}

void loop() {
  char key = keypad.getKey();
  if (x == 1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("rfid correct");
    delay(1000);
    Serial.println("password is correct");
    x = 0;
    door_open();

  }
  else if (x==2){
     lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("rfid not correct");
    delay(1000);
    Serial.println("password is correct");
    x = 0;
  }
  if (key) {
    Serial.println(key);
    input_password += key;          // append new character to input password string
    lcd.setCursor(cursorColumn, 0); // move cursor to   (cursorColumn, 0)
    if (key != '*')
    {
    lcd.print("*");                 // print key at (cursorColumn, 0)
    }
    cursorColumn++;
    Serial.println("input_password.length :");
    Serial.println(input_password.length());
    Serial.println("");
    delay(1000);
    if (key == '*')
    {
      input_password = ""; // clear input password
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("enter pass:");
      lcd.setCursor(0, 1);
      lcd.print("* clear ");
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
        x = 0;
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
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("enter pass:");
        lcd.setCursor(0, 1);
        lcd.print("* clear ");

        cursorColumn = 11;
        Serial.println("password is incorrect, try again");
        pass_count = 0;
      }

      input_password = ""; // clear input password
    }
    else
    {

      //input_password += key; // append new character to input password string

    }
  }
}
// function that executes whenever data is received from master

void receiveEvent() {
  char c ;

  while (0 < Wire.available()) {
    c = Wire.read();      /* receive byte as a character */
    Serial.print("i2c receved=");
    delay(500);
    Serial.print(c);            /* print the character */
    //Serial.print("done!!!");
    x = (int)(c);
    x = x - 48;
    Serial.print(x);
  }

  Serial.println();             /* to newline */
}
void door_open() {
  //**********************************DO YOUR WORK HERE********************
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("door is  open..");
  
  //***********************************************************************
}
