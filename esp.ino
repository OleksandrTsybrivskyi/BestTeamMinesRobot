#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Galaxy S21 Ultra 5G 65a2";       // Введи свій WiFi SSID
const char* password = "88888888";   // І пароль

WiFiClient client;
String incomingData = "";

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  // Serial.println("Connecting to WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    // Serial.print(".");
  }

  // Serial.println("Connected!");
}

void loop() {
  // Прочитали з UART JSON з Arduino
  if (Serial.available()) {
    String telemetry = Serial.readStringUntil('\n');
    telemetry.trim();
    sendTelemetry(telemetry);
  }

  // Питаємо команду
  String command = getCommand();
  if (command != "") {
    Serial.println(command);  // Відправили назад на Arduino
  }

  delay(100); // щоб не спамити сервер
}

void sendTelemetry(String jsonPayload) {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  http.begin("http://example.com");
  http.addHeader("Content-Type", "application/json");

  http.POST(jsonPayload);  // нам неважлива відповідь
  http.end();
}
String getCommand() {
  if (WiFi.status() != WL_CONNECTED) return "wifi_error";

  HTTPClient http;
  http.begin("http://example.com");  // адреса може бути та сама або інша, якщо треба
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    String response = http.getString();
    http.end();

    response.trim();
    if (response == "null") return "";  // немає оновлення

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, response);
    if (!error && doc.containsKey("data")) {
      return doc["data"].as<String>();
    } else {
      return "parse_error";
    }
  } else {
    http.end();
    return "get_failed";
  }
}
