/**************************************************/
/* Author     :keroles girgis                     */
/* Date       :20-1-2021                          */
/* last_edit  :06-4-2021                          */
/* version    :0.5 V                              */
/* Description:esp8266 wifi+ relay                */
/**************************************************/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID          "TP-LINK_5A42"
#define WLAN_PASS          "32735024"
#define mqtt_serv_address  "192.168.0.103"        // ip for local server(rasperipi)or link to test server  
#define mqtt_port_number    1883
#define relay_pin             D0  				   //relay conected to pin 10  
#define dh11_pin              D1
#define lamp                  D2
DHT dht;
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
void callback(String topic, byte* payload, unsigned int msg_length)
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
 	if(topic=="room/lamp")
 	{
    
      Serial.print("Changing Room lamp to ");
      if(messageTemp == "on"){
        digitalWrite(lamp, HIGH);
        Serial.print("On");
      }
      else if(messageTemp == "off"){
        digitalWrite(lamp, LOW);
        Serial.print("Off");
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
void setup() 
{
  
  Serial.begin(115200);
  
  dht.setup(dh11_pin);

  setup_wifi();
/************************* pin modes configuration *********************************/
  pinMode(relay_pin, OUTPUT);
  pinMode(lamp, OUTPUT);
/*********************start by connecting to a Wi-Fi*******************/
  client.setServer(mqtt_serv_address, mqtt_port_number); // set ip as a local server 
  client.setCallback(callback);
}
void loop() 
{

	if (!client.connected()) 
	{                             //if not connected try to reconnect 
		reconnect();
  }
	client.loop();

	long now = millis();                                   // current time in millis second.

	if (now - lastMsg> 2000)
	{
		lastMsg = now;
		//String relay_state;
		 float h = dht.getHumidity();
   		 // Read temperature as Celsius (the default)
    	float t = dht.getTemperature();
    	// Read temperature as Fahrenheit (isFahrenheit = true)
    	float f = dht.toFahrenheit(t);
    	if (isnan(h) || isnan(t) || isnan(f)) 
    	{
    		Serial.println("Failed to read from DHT sensor!");
   			return;
   		}
/*
		if (digitalRead(relay_pin)== HIGH)
		{
			relay_state="ON";
		}
			else
		{
			relay_state="OFF";		
		}
*/
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

   }
}




