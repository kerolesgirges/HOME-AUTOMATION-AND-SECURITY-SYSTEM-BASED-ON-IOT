
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>

#define denied 2
#define rif 3
#define finger_print 4

#define RST_PIN  D3     // Configurable, see typical pin layout above
#define SS_PIN  D4     // Configurable, see typical pin layout above

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;

String tag;

void setup() {
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
  rfid.PCD_Init(); // Init MFRC522
  pinMode(D8, OUTPUT);
}

void loop() {
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
      Wire.write("1");  /* sends hello string */
      Wire.endTransmission();    /* stop transmitting */

      delay(2000);
    } else {
      Serial.println("Access Denied!");
      Wire.beginTransmission(8); /* begin with device address 8 */
      Wire.write("2");  /* sends hello string */
      Wire.endTransmission();    /* stop transmitting */
      
      delay(2000);
      
    }
    tag = "";
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}
