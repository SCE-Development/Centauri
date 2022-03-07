#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"

//Constants
#define AWS_IOT_PUBLISH_TOPIC   "MessageForNode"
#define AWS_IOT_SUBSCRIBE_TOPIC "MessageForESP32"

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);


void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi");
  
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);

  // Create a message handler
  client.setCallback(messageHandler);

  Serial.print("Connecting to AWS IOT...");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }

  if(!client.connected()){
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}

void publishMessage(int rfid_byte)
{
  StaticJsonDocument<200> doc;
  doc["message"] = rfid_byte;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void messageHandler(char* topic, byte* payload, unsigned int length) {
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const int message = doc["message"];
  if(message == 200) {
   Serial.println("Door Open");
  } else {
    Serial.println("Unauthorized");
  }
}

void setup() {
  //Init Serial USB
  Serial.begin(9600);
  Serial.println(F("Initialize System"));
  //Connect to AWS IoT
  connectAWS();
}

void loop() {
   digitalWrite(PwrPin,HIGH);
  readRFID();
  //listen for messages from aws iot
  client.loop();
}


String toString(byte *buffer, byte bufferSize) {
  String ret = "";
   for (byte i = 0; i < bufferSize; i++) {
    ret += buffer[i];
  }
  return ret;
}
