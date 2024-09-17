#include <WiFi.h>
#include <WebSocketsClient.h>
#include <Wire.h>
#include <TinyGPS++.h>
#include <ArduinoJson.h>

// Reemplaza con tus credenciales de WiFi
const char* ssid = "MEGACABLE-2.4G-08AB";
const char* password = "M3hXzNM3Kx";

// Dirección IP y puerto del servidor Node.js 
const char* webSocket_server_host = "192.168.100.21";
const int webSocket_server_port = 1234;

WebSocketsClient webSocket;
TinyGPSPlus gps;

#define RXD2 16 // Ajusta este pin según tu configuración física
#define TXD2 17 // Ajusta este pin según tu configuración física
HardwareSerial neogps(2); // Utilizamos la segunda UART en el ESP32

unsigned long lastSendTime = 0;
const unsigned long sendInterval = 10000; // Intervalo de 10 segundos

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WebSocket] Desconectado\n");
      break;
    case WStype_CONNECTED:
      Serial.printf("[WebSocket] Conectado al servidor\n");
      break;
    case WStype_TEXT:
      Serial.printf("[WebSocket] Mensaje recibido: %s\n", payload);
      break;
  }
}

void setup() {
  Serial.begin(115200);
  neogps.begin(9600, SERIAL_8N1, RXD2, TXD2); // Verifica la velocidad de transmisión del GPS
  delay(5000); // Agrega un retardo de 5 segundos para que el GPS obtenga señal

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado a WiFi");

  webSocket.begin(webSocket_server_host, webSocket_server_port);
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();

  if (millis() - lastSendTime >= sendInterval) {
    lastSendTime = millis();

    if (gps.location.isValid()) {
      StaticJsonDocument<512> doc;
      JsonObject gpsData = doc.to<JsonObject>();
      gpsData["lat"] = gps.location.lat();
      gpsData["lng"] = gps.location.lng();
      gpsData["latDeg"] = String(gps.location.lat(), 6).substring(0, 2);
      gpsData["latMin"] = String(gps.location.lat(), 6).substring(2, 8);
      gpsData["lngDeg"] = String(gps.location.lng(), 6).substring(0, 3);
      gpsData["lngMin"] = String(gps.location.lng(), 6).substring(3, 9);
      gpsData["speed"] = gps.speed.kmph();
      gpsData["altitude"] = gps.altitude.meters();
      gpsData["year"] = gps.date.year();
      gpsData["month"] = gps.date.month();
      gpsData["day"] = gps.date.day();
      gpsData["hour"] = gps.time.hour();
      gpsData["minute"] = gps.time.minute();
      gpsData["second"] = gps.time.second();
      gpsData["satellites"] = gps.satellites.value();

      String jsonString;
      serializeJson(doc, jsonString);
      webSocket.sendTXT(jsonString);
    } else {
      Serial.println("Sin señal GPS");
    }
  }

  while (neogps.available()) {
    if (gps.encode(neogps.read())) {
      // Nuevos datos GPS disponibles
    }
  }
}
