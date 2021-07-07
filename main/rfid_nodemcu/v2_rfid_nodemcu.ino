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
*/
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#define denied 2
#define rif 3
#define finger_print 4

#define RST_PIN  D3     // Configurable, see typical pin layout above
#define SS_PIN  D4      // Configurable, see typical pin layout above

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;

String tag;

void setup() {
  
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
 // SPI.setup(0);
  Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
  rfid.PCD_Init(); // Init MFRC522
  pinMode(D8, OUTPUT);
}
// x=1 rfid correct
// x=2 rfid not correct
// x=3 finger_is_correct
// x=4 finger_is'n_correct
// x=5 door_is_closed from magnitic sensor
void loop() {
  //Serial.print(millis());
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
