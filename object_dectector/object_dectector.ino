#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Servo.h> //servo library

Servo servo;
const char *SSID = "HSU_Students";
const char *PASSWORD = "dhhs12cnvch";
const char *URL = "http://10.106.25.65:3000/object";

// SENSOR DECLARED
const int echoPin = D7;
const int trigPin = D6;
int servoPin = D5;

//HR-SC04 SECTION
long duration, inches, cm;
char message[25] = "";

ESP8266WiFiMulti WiFiMulti;
WiFiClient client;
HTTPClient http;


long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}

void setup() {
  Serial.begin(115200);

  servo.attach(servoPin);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  servo.write(0);  //close cap on power on
  delay(100);
  servo.detach();
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(SSID, PASSWORD);
  // wait for WiFi connection
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(1000);
}

void sensor() {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(15);
  digitalWrite(trigPin, LOW);
  pinMode(echoPin, INPUT);

  duration = pulseIn(echoPin, HIGH);
  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);

  if (cm < 30 || inches < 11.8110236) {
    servo.attach(servoPin);
    delay(1);
    servo.write(0);
    delay(5000);
    servo.write(180);
    delay(1000);
    servo.detach();
    strcpy(message, "Dustbin open");
  } else {
    strcpy(message, "Dustbin closed");
  }
}

void postJsonData() {

  Serial.print("connecting to ");
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    Serial.print("[HTTP] begin...\n");
    if (http.begin(client, URL)) {  // HTTP
      Serial.print("[HTTP] POST...\n");
      //gui du lieu len server dang JSON
      const int capacity = JSON_OBJECT_SIZE(5);
      StaticJsonDocument<capacity> doc;


      doc["message"] = message;
      doc["cm"] = cm;
      doc["inches"] = inches;

      char output[2048];
      serializeJson(doc, Serial);
      serializeJson(doc, output);


      http.addHeader("Content-Type", "application/json");
      int httpCode = http.POST(output);
      Serial.println(httpCode);  //Print HTTP return code
      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        // String payload = http.getString();
        // Serial.println(payload);
        // char input[50];
        // payload.toCharArray(input, 50);
        // //parseJson
        // Serial.println("Begin parse json data ...");
        // DynamicJsonDocument docResponse(2048);
        // DeserializationError err = deserializeJson(docResponse, payload);
        // if (err) {
        //   Serial.print(F("deserializeJson() failed with code "));
        //   Serial.println(err.c_str());
        // }
        //auto name = docResponse["name"].as<char*>();
        Serial.println("done");

      } else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();  //Close connection Serial.println();
      Serial.println("closing connection");
    }
  }
}

void loop() {
  sensor();
  postJsonData();
  delay(1500);
}