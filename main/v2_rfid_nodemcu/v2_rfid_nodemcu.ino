/*
connections
rfid -----> nodemcu
sda-----> D4
sck-----> D5
mosi----->D7
miso----->D6
gnd-----> GND 
rst----->D3
3.3v----->3V3
*************************
D8 ---->pin 2 ardiuno
*/
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#define denied 2
#define rif 3
#define finger_print 4
#define door_lock D8
#define RST_PIN  D3     // Configurable, see typical pin layout above
#define SS_PIN  D4      // Configurable, see typical pin layout above

int door_open_count = 0;   // counter for the number of door_opend
int door_stat = 0;         // current state of the door
int lastdoor_stat = 0;     // previous state of the door

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;
int tick;
String tag;

void setup() {
  
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
 // SPI.setup(0);
  Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
  rfid.PCD_Init(); // Init MFRC522
  pinMode(door_lock,INPUT );
}
// x=1 rfid correct
// x=2 rfid not correct
// x=3 finger_is_correct
// x=4 finger_is'n_correct
// x=5 door_is_closed from magnitic sensor
void loop() {

  door_stat= digitalRead(door_lock);
  if (door_stat != lastdoor_stat)
  {
    if (door_stat ==HIGH) // door is open 
    {
      Serial.println("door is open ");
      Serial.println("door_open_count=");
      Serial.println(door_open_count);

      door_open_count++;  // count num of open door times
    }
    else{ //door is closed 
      Serial.println("door is closed ");
    }
  }
  lastdoor_stat = door_stat;
tick++;
if (tick==500){  // just indication its working probably 
  Serial.print(millis());
  tick=0;
  }
  if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];
    }
    Serial.println(tag);
    if (tag == "156116174137") {
      Serial.println("Access Granted!");
      Wire.beginTransmission(8); /* begin with device address 8 */
      Wire.write("1");           /* rfid correct send 1 */
      Wire.endTransmission();    /* stop transmitting */

      delay(2000);
    } else {
      Serial.println("Access Denied!");
      Wire.beginTransmission(8); /* begin with device address 8 */
      Wire.write("2");           /* rfid not correct */
      Wire.endTransmission();    /* stop transmitting */
      delay(2000);

    }
    tag = "";
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}
