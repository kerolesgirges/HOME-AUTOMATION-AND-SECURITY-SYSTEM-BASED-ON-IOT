/**************************************************/
/* Author     :keroles girgis                     */
/* Date       :13-7-2021                          */
/* last_edit  :--2021                             */
/* version    :0.1 V                              */
/* Description:esp8266 wifi+ door mode            */
/**************************************************/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
/************************* WiFi Access Point *********************************/

#define WLAN_SSID          "pronto"
#define WLAN_PASS          "zxc123asd"
#define mqtt_serv_address  "192.168.0.103"        // ip for local server(rasperipi)or link to test server  
#define mqtt_port_number    1883
#define relay_pin             D0  				   //relay conected to pin 10  
#define dh11_pin              D1
#define lamp                  D2
/***************************door_status********************/
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

/*make this esp as a clint in local server  */

WiFiClient espClient;
PubSubClient client(espClient); 
long lastMsg = 0;
char msg[50];
int value = 0;


void setup_wifi()
{

  delay(10);
  Serial.println(); 
  Serial.println();
	Serial.print("Connecting to ");
	Serial.println(WLAN_SSID);

	WiFi.begin(WLAN_SSID, WLAN_PASS);

	while (WiFi.status() != WL_CONNECTED)
	{   		//while not conected loop unteil connecting  
    
    	WiFi.begin(WLAN_SSID, WLAN_PASS);
    	Serial.print(".");
    	delay(5000);
  }

	Serial.println("WiFi connected");
	Serial.println("IP address: "); 
	Serial.println(WiFi.localIP());

}
void callback(String topic, byte* payload, unsigned int msg_length)  //receve massege from server 
{

	Serial.print("Message arrived on topic :");
  	Serial.print(topic);
  	Serial.print(". Message: ");
  	String messageTemp; 
  	for (int i = 0; i< msg_length; i++) 
 	{
    	Serial.print((char)payload[i]);   				// printing all the message from 0 to i
    	messageTemp += (char)payload[i];
  	}
  	Serial.println();
  	// Switch relay mode as  was received as first character in payload packet 
  	/*if ((char)payload[0] == '0') 
  	{
  		digitalWrite(relay_pin, LOW);  						// Turn the relay off } 
 	}
  	else
  	{
  		digitalWrite(relay_pin, HIGH); 						// Turn the relay on }
 	}
  */
 	if(topic=="front/door")
 	{
    
      Serial.print("Changing door to ");
      if(messageTemp == "open"){
        
        Serial.println("Access Granted from server");
        Wire.beginTransmission(8); /* begin with device address 8 */
        Wire.write("4");           /* face is correct correct send 4 */
        Wire.endTransmission();    /* stop transmitting */
        
        Serial.print("Open");
      }
      else if(messageTemp == "closed"){

        //digitalWrite(lamp, LOW);
        
        Serial.print("door is closed");
      }
  }
  Serial.println();

}

void reconnect()
{           	            	// Loop until we're reconnected

	while (!client.connected()) 
	{
		Serial.print("Attempting MQTT connection...");// Attempt to connect

		if (client.connect("ESP8266Client"))
		{
			Serial.println("connected");
			// Once connected, publish an announcement...
		} 
		else
		{
			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 5 seconds");
			// Wait 5 seconds before retrying
			delay(5000);
   	 	}
   	 	client.subscribe("esp/test1");
       client.subscribe("room/lamp");
		//client.publish("fromEsp8266", "Hello world, I am ESP8266!");
		
   }
}
void door_mode_detact_send(){
	door_stat= digitalRead(door_lock);
  if (door_stat != lastdoor_stat)
  {
    if (door_stat ==HIGH) // door is open 
    {
      
			client.publish("front/door", "open");
      Serial.println("door is open ");
      //Serial.println("door_open_count=");
      //Serial.println(door_open_count);

      door_open_count++;  // count num of open door times
    }
    else{ //door is closed 
    	client.publish("front/door", "closed");
      Serial.println("door is closed ");
    }
  }
  lastdoor_stat = door_stat;
}
void setup() 
{
  
  //Serial.begin(115200);
  
  Serial.begin(9600);
  SPI.begin();        // Init SPI bus
  Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
  rfid.PCD_Init();    // Init MFRC522
  setup_wifi();
/************************* pin modes configuration *********************************/
  pinMode(door_lock,INPUT );
  //pinMode(relay_pin, OUTPUT);
  //pinMode(lamp, OUTPUT);
/*********************start by connecting to a Wi-Fi*******************/
  client.setServer(mqtt_serv_address, mqtt_port_number); // set ip as a local server 
  client.setCallback(callback);
}
// door_stat=1 rfid correct
// door_stat=2 rfid not correct
// door_stat=3 finger_is_correct
// door_stat=4 face recognation correct
// door_stat=5 
void loop() 
{
	
	if (!client.connected()) 
	{                             //if not connected try to reconnect 
		reconnect();
  }
	client.loop();

	door_mode_detact_send();
	
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
	/*
	long now = millis();                                   // current time in millis second.
  
	if (now - lastMsg> 2000)                       // every two second send messge 
	{
		lastMsg = now;
		String relay_state;
		 
      //client.publish("topic", char value);
   		

  	  Serial.println("Humidity: ");
  		Serial.print(h);
    	Serial.println(" %\t Temperature: ");
    	Serial.print(t);
    	Serial.println(" *C ");
    	
      client.publish("room/temperature", String(t).c_str());
      client.publish("room/humidity", String(h).c_str());
      client.subscribe("room/lamp");
	//	Serial.print("Publish message: ");
	//	Serial.println(relay_state);
    //  client.publish("esp/test1/status", relay_state.c_str()); 
    
		//client.publish("outdoor/relay/status", relay_state.c_str()); 
		// .c_str >>  Converts the contents of a String as a C-style,Returns
		//A pointer to the C-style version of the invoking String.

		/*
		++value;
		snprintf (msg, 75, "Hello world #%ld", value);
		Serial.print("Publish message: ");
		Serial.println(msg);
		client.publish("fromEsp8266_node _1", password);       // publish data (msg) with topic fromEsp8266
		*/

   }*/
}




