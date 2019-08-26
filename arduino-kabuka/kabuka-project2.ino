//#include "certs.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

// The name of the device. This MUST match up with the name defined in the AWS console
#define DEVICE_NAME "kabuka-device"

// The MQTTT endpoint for the device (unique for each AWS account but shared amongst devices within the account)
#define AWS_IOT_ENDPOINT "a9phn94b3kj28-ats.iot.us-east-1.amazonaws.com"

// The MQTT topic that this device should publish to
#define AWS_IOT_TOPIC "kabuka-device/door/parentroom"

// How many times we should attempt to connect to AWS
#define AWS_MAX_RECONNECT_TRIES 24

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

const char* ssid = "YOUR WIFI SSID";
const char* password = "YOUR WIFI PASSWORD";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

const int doorsensor = 5;
int sensorstate = LOW;

void connectToWiFi(){
  WiFi.mode(WIFI_STA);
  Serial.println("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}

void connectToAWS(){
    // Configure WiFiClientSecure to use the AWS certificates we generated
    net.setCACert(AWS_CERT_CA);
    net.setCertificate(AWS_CERT_CRT);
    net.setPrivateKey(AWS_CERT_PRIVATE);

    // Connect to the MQTT broker on the AWS endpoint we defined earlier
    client.begin(AWS_IOT_ENDPOINT, 8883, net);
    while (!client.connect(DEVICE_NAME)) {
        Serial.print(".");
        delay(500);
    }
    // Make sure that we did indeed successfully connect to the MQTT broker
    // If not we just end the function and wait for the next loop.
    if(!client.connected()){
        Serial.println(" Timeout!");
        return;
    }
    // If we land here, we have successfully connected to AWS!
    // And we can subscribe to topics and send messages.
    Serial.println("Connected to AWS!");
}

String timeStamp(){
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  String time = timeClient.getFormattedDate();
  return time;  
}

void sendDoorOpenedToAWS(){
    StaticJsonDocument<128> jsonDocOpen;
    jsonDocOpen["message"] = "Parent room door was OPENED.";
    jsonDocOpen["time"] = timeStamp();
    jsonDocOpen["state"] = "opened";
    char jsonBufferOpen[512];
    serializeJson(jsonDocOpen, jsonBufferOpen);   
    client.publish(AWS_IOT_TOPIC, jsonBufferOpen);
//    Serial.println("Publishing:");
//    serializeJson(jsonDocOpen, Serial);
//    Serial.println();
}

void sendDoorClosedToAWS(){
    StaticJsonDocument<128> jsonDocClose;
    jsonDocClose["message"] = "Parent room door was closed.";
    jsonDocClose["time"] = timeStamp();
    jsonDocClose["state"] = "closed";
    char jsonBufferClose[512];
    serializeJson(jsonDocClose, jsonBufferClose);
    client.publish(AWS_IOT_TOPIC, jsonBufferClose);
//    Serial.println("Publishing:");
//    serializeJson(jsonDocClose, Serial);
//    Serial.println();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(doorsensor, INPUT_PULLUP);
  connectToWiFi();
  connectToAWS();
  timeClient.begin();
  timeClient.setTimeOffset(25200);
}

void loop() {
  // put your main code here, to run repeatedly:
  int value = digitalRead(doorsensor);

  if (value) {
    Serial.println("Door was opened");
    if (sensorstate == LOW) {
      sendDoorOpenedToAWS();
      sensorstate = HIGH;
    }  
  } else {
    Serial.println("Door was closed");
    if (sensorstate == HIGH) {
      sendDoorClosedToAWS();
      sensorstate = LOW;
    }
  }
  client.loop();
  delay(1000);
}
