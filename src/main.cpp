#include <Arduino.h>
#include <Arduino_JSON.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <AsyncTCP.h>
#include "SPIFFS.h"
#include <WebSocketsServer.h>

const char* ssid = "liyas ferdy";  // Ganti dengan SSID WiFi Anda
const char* password = "yeay010101";  // Ganti dengan password WiFi Anda

WebSocketsServer webSocket = WebSocketsServer(81);

//Sensor initiated here
const int turbidityPin = 33;
const int tdsPin = 34;

int turbidity;
int tds;


String value;
unsigned long lastTime = 0;
unsigned long timerDelay = 100;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
JSONVar readings;

float getRandomValue(float minValue, float maxValue) {
  return random(minValue * 10, maxValue * 10) / 10.0; // Mendapatkan nilai desimal
}



String getSensorReadings() {
  readings["temperature"] = String(getRandomValue(20, 30)); // Nilai temperatur antara 20 dan 30
  readings["turbidity"] = String(turbidity);    // Nilai kelembapan antara 40 dan 60
  readings["tds"] = String(tds); // Nilai potensiometer antara 0 dan 100

  String jsonString = JSON.stringify(readings);
  return jsonString;
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        webSocket.sendTXT(num, "0");
      }
      break;
  }
}

void initSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

void setup() {
  Serial.begin(115200);
  initSPIFFS();

  

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP());

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(getRandomValue(20, 30)).c_str());
  });

  server.on("/turbidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(turbidity).c_str());
  });

  server.on("/tds", HTTP_GET, [](AsyncWebServerRequest *request){  
    request->send_P(200, "text/plain", String(tds).c_str());
  });

  server.serveStatic("/", SPIFFS, "/");

  server.begin();
}

String sentData;

void loop() {

  webSocket.loop();

  // Update nilai angka dengan nilai acak pada setiap iterasi loop
  float angka = getRandomValue(40, 60); // Ganti rentang nilai sesuai kebutuhan
  turbidity = analogRead(turbidityPin);
  tds = analogRead(tdsPin);

  if ((millis() - lastTime) > timerDelay) {
    sentData = getSensorReadings();
    webSocket.broadcastTXT(sentData);
    lastTime = millis();
  }
}
