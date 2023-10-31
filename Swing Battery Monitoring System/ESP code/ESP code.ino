#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define FIREBASE_HOST "https://testvoltlogs-default-rtdb.firebaseio.com/"
#define FIREBASE_DB_SECRET "T6dZLP4E8b2eiLEQPor88vW4Ln8axiIiXKo9ZHU6"
#define FIREBASE_PROJECT_ID "testvoltlogs"

FirebaseData firebaseData;
FirebaseJson json;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");


int start, stop;

void setup() {

  Serial.begin(9600);

  WiFi.mode(WIFI_STA);
  WiFi.begin("Infinix NOTE 8", "090526921");

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
  }
  Serial.write('C');

  Firebase.begin(FIREBASE_HOST, FIREBASE_PROJECT_ID);
  Firebase.reconnectWiFi(true);

  timeClient.begin();
  timeClient.setTimeOffset(0);
}

void loop() {
  timeClient.update();

  if (Serial.available()) {
    sendGETRequest(Serial.readStringUntil('\n'));
  }
  delay(1000);
}

void sendGETRequest(String data) {
  String dataValue[3];

  for (byte i = 0; i < 3; i++) {
    start = 0;
    stop = data.indexOf(":");
    dataValue[i] = data.substring(start, stop);

    start = stop + 1;
    data = data.substring(start, data.length());
  }

  json.set("charge_current", dataValue[0]);
  json.set("load_current", dataValue[1]);
  json.set("battery_voltage", dataValue[2]);
  json.set("time_stamp", timeClient.getEpochTime());

  if (Firebase.push(firebaseData, "/data", json)) {
    Serial.write('Y');
  } else {
    Serial.write('N');
  }
}