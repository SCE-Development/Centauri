#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"

#define AWS_IOT_PUBLISH_TOPIC   "MessageForNode"
#define AWS_IOT_SUBSCRIBE_TOPIC "MessageForESP32"

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

/**
 * Connects to WiFi using credentials given in
 * secrets.h file
*/
void connectToWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  if(WiFi.status() == WL_CONNECTED) {
    Serial.println("...Connected");
  }
}

/**
 * Connects to AWS IoT Core using the 
 * credentials provided in secrets.h file
 * and sets publish and subscribe topic
*/
void connectToAWS() {
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);
  client.setCallback(HandleMessageFromPubSub);
  Serial.print("Connecting to AWS IOT...");
  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }
  if(!client.connected()){
    Serial.println("AWS IoT Timeout!");
    return;
  }
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
  Serial.println("AWS IoT Connected!");
}

/**
 * Published scanned RFID's byte to AWS to be validated
 * by the RFID API running in Core-v4
 * e.g. :
 *      {
 *        "message" : <bytes>  
 *      }
 * where bytes is the RFID bytes stored as an integer
*/
void publishMessage(int rfid_byte)
{
  StaticJsonDocument<200> doc;
  doc["message"] = rfid_byte;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

/**
 * Handles incoming messages and determines
 * if door should be opened or not based on the
 * message recieved
 * 
 * e.g. message format: 
 *    {
 *      "message" : <int> 
 *    }
 * where <int> is HTTP code
*/
void HandleMessageFromPubSub(char* topic, byte* payload, unsigned int length) {
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const int message = doc["message"];
  if(message == 200) {
   Serial.println("Door Open");
  } else {
    Serial.println("Unauthorized");
  }
}

/**
 * Converts RFID bytes array to String
*/
String toString(byte *buffer, byte bufferSize) {
  String ret = "";
   for (byte i = 0; i < bufferSize; i++) {
    ret += buffer[i];
  }
  return ret;
}

void setup() {
  Serial.begin(9600);
  Serial.println(F("Initialize System"));
  connectToWiFi();
  connectAWS();
}

void loop() {
   digitalWrite(PwrPin,HIGH);
  readRFID();
  client.loop();
}
