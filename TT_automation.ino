//#include <Ultrasonic.h>
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <HX711_ADC.h> // https://github.com/olkal/HX711_ADC
#include <Wire.h>
/*//#include "DHTesp.h"
//#define LED1 D6
//#define  ac D2
//const int SenOut = A0;
//#define LED2 D7*/
/*#define ac D2
#define buzzer D5
#define machine D7
#define motor D1
#define ir D6
#define gas D8*/
//Ultrasonic ultrasonic(D4, D3);
//DHTesp dht;
//int sensorValue = 0;
//int D8,D7;
HX711_ADC LoadCell(D8, D7);
const int trigP = 2;  //D4 Or GPIO-2 of nodemcu
const int echoP = 0;  //D3 Or GPIO-0 of nodemcu
int distance1;
long duration;
int distance;
/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "OPPO"
#define WLAN_PASS       "oppophone"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "shriram_06"
#define AIO_KEY         "db72a86e8d534cf8b75c2e85276ef52f"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish LC = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/artengo");
Adafruit_MQTT_Publish US = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/BALL");
//Adafruit_MQTT_Publish temp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");
//Adafruit_MQTT_Publish us = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/US");
//Adafruit_MQTT_Publish obs = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/IR");
//Adafruit_MQTT_Publish gs = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/GAS");
// Setup a feed called 'onoff' for subscribing to changes.
//Adafruit_MQTT_Subscribe light1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/trial");
//Adafruit_MQTT_Subscribe light2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/L2");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup()
{
  Serial.begin(115200);
  //delay(10);  
  LoadCell.begin(); // start connection to HX711
  LoadCell.start(2000); // load cells gets 2000ms of time to stabilize
  LoadCell.setCalFactor(999.0);
  pinMode(trigP, OUTPUT);  // Sets the trigPin as an Output
pinMode(echoP, INPUT);   // Sets the echoPin as an Input
Serial.begin(115200);      // Open serial channel at 9600 baud rate
    
 // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);


  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  //mqtt.subscribe(&light1);
//  mqtt.subscribe(&light2);
}

//uint32_t x=analogRead(A0);
//uint32_t  temp=0;

void loop()
{
  
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop

  // try to spend your time here
delay(4000);

   
LoadCell.update(); // retrieves data from the load cell
 float i = LoadCell.getData(); 
 Serial.print(i);
  Serial.print(F("\nSending value of Bat: "));
  Serial.print(i);
  Serial.print("...");
  if (! LC.publish(i)) 
  {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  digitalWrite(trigP, LOW);   // Makes trigPin low
delayMicroseconds(2);       // 2 micro second delay 

digitalWrite(trigP, HIGH);  // tigPin high
delayMicroseconds(10);      // trigPin high for 10 micro seconds
digitalWrite(trigP, LOW);   // trigPin low

duration = pulseIn(echoP, HIGH);   //Read echo pin, time in microseconds
distance= duration*0.034/2;        //Calculating actual/real distance
if (distance == 11 or distance == 12)
{
  distance1 = 0;
}
if (distance ==8 or distance == 9)
{
  distance1 = 1;
}
if (distance == 5 or distance == 4)
{
  distance1=2;
}
delay(3000);

 Serial.println(distance);
   Serial.print(F("\nSending value of ball "));
  Serial.print(distance);
  Serial.print("...");
  if (! US.publish(distance1)) 
  {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
/*sensorValue = analogRead(SenOut);
  Serial.println(sensorValue);
Serial.print(F("\nSending dB val "));
  Serial.print(sensorValue);
  Serial.print("...");
  if (! sound.publish(sensorValue)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
/*uint32_t  x=0;
  float temperature = dht.getTemperature();
  x=temperature;
  Serial.println(temperature);
  if(temperature>25)
  {
    digitalWrite(ac,1);
  }
  else
  {
    digitalWrite(ac,0); 
  }

  
  //int LED1;
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &light1) {
      Serial.print(F("Got: "));
      Serial.println((char *)light1.lastread);
      int light1_state=atoi((char *)light1.lastread);
      digitalWrite(LED1,light1_state);
    }
    }/*
    if (subscription == &light2) {
      Serial.print(F("Got: "));
      Serial.println((char *)light2.lastread);
      int light2_state=atoi((char *)light2.lastread);
      digitalWrite(LED2,light2_state);
    }
  }*/
/*
  // Now we can publish stuff!
  Serial.print(F("\nSending temperature val "));
  Serial.print(x);
  Serial.print("...");
  if (! temp.publish(x)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
  
  /*Serial.print("Distance in CM: ");
  // Pass INC as a parameter to get the distance in inches
  if(ultrasonic.distanceRead()==357)
  {
    Serial.println("0");
  }
  else
  {
    Serial.println(ultrasonic.distanceRead());
  }
  delay(1000);
  if(ultrasonic.distanceRead()<30)
  {
    digitalWrite(buzzer,1);
  }
  else
  {
    digitalWrite(buzzer,0); 
  }
  if (! us.publish(ultrasonic.distanceRead())) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
  int obstacle = digitalRead(ir);
  Serial.println(obstacle);
  if(obstacle == LOW )
  {
    digitalWrite(machine,0);
  }
  else
  {
    digitalWrite(machine,1);
    
  
  }
  if (! obs.publish(obstacle)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
  */
  /*int gas_value= digitalRead(gas);
  Serial.println("Gas:");
  Serial.println(gas_value);
  if(gas_value ==0)
  {
    digitalWrite(motor,1);
  }
  else
  {
    digitalWrite(motor,0);
  }
  if (! gs.publish(gas_value)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }*/
  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  /*
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  */
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
