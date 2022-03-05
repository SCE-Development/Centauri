//Libraries
#include <SPI.h>//https://www.arduino.cc/en/reference/SPI
#include <MFRC522.h>//https://github.com/miguelbalboa/rfid
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"

//Constants
#define SS_PIN 5
#define RST_PIN 0
#define PwrPin 22
#define AWS_IOT_PUBLISH_TOPIC   "MessageForNode"
#define AWS_IOT_SUBSCRIBE_TOPIC "MessageForESP32"

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

//Variables
byte nuidPICC[4] = {0, 0, 0, 0};
MFRC522::MIFARE_Key key;
MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);

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
  //init rfid D8,D5,D6,D7
  SPI.begin();
  rfid.PCD_Init();
  Serial.print(F("Reader :"));
  rfid.PCD_DumpVersionToSerial();
  pinMode(PwrPin, OUTPUT);
}

void loop() {
   digitalWrite(PwrPin,HIGH);
  readRFID();
  //listen for messages from aws iot
  client.loop();
}

void readRFID(void ) { /* function readRFID */
  ////Read RFID card
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  
  int rfid_byte = toString(rfid.uid.uidByte, rfid.uid.size).toInt();
  publishMessage(rfid_byte);

  
  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

String toString(byte *buffer, byte bufferSize) {
  String ret = "";
   for (byte i = 0; i < bufferSize; i++) {
    ret += buffer[i];
  }
  return ret;
}
