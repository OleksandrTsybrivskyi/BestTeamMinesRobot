#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
// #include <MPU6050.h>
// #include <SoftwareSerial.h>

const char* ssid = "namew";       // Введи свій WiFi SSID
const char* password = "11111111";   // І пароль

WiFiClient client;
String incomingData = "";

// SoftwareSerial espSerial(4, 0);

// const int echoPin = 8;
// const int trigPin = 9;
int motorRightPin1 = 16;                                                 
int motorRightPin2 = 14;
int motorLeftPin1 = 12;
int motorLeftPin2 = 13;

// float distance = 100.0;
// float newDistance;

long counter = 0;

// int16_t ax = 1, ay = 1, az = 1;
// MPU6050 mpu;

void setup() {
  Serial.begin(9600);
  // espSerial.begin(9600);
  Serial.println(-4);
  WiFi.begin(ssid, password);
  // Serial.println("Connecting to WiFi...");
  Serial.println(-3);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // ULTRASOUND SENSOR
  // pinMode(trigPin, OUTPUT);
  // pinMode(echoPin, INPUT);

  // MOTORS
  pinMode(motorRightPin1, OUTPUT);
  pinMode(motorRightPin2, OUTPUT);
  pinMode(motorLeftPin1, OUTPUT);
  pinMode(motorLeftPin2, OUTPUT);
  Serial.println(-2);
  // mpu.initialize();
  Serial.println(-1);

  pinMode(A0, INPUT);
}

void loop() {
  // Прочитали з UART JSON з Arduino
  // if (Serial.available()) {
  //   String telemetry = Serial.readStringUntil('\n');
  //   telemetry.trim();
  //   sendTelemetry(telemetry);
  // }

  // Питаємо команду
  String cmd = getCommand();
  Serial.print(counter);
  counter++;
  Serial.print(" command = ");
  Serial.println(cmd);
  if (cmd == "rotateLeft") rotateCounterClockwise();
  if (cmd == "rotateRight") rotateClockwise();
  if (cmd == "moveForward") forwardMove();
  if (cmd == "moveBackward") backwardMove();
  if (cmd == "stopEverything") stopMotors();
  // else stopMotors();
  cmd = "";

  // newDistance = distanceUltraSoundSensor(trigPin, echoPin);
  // distance = 0.1 * newDistance + 0.9 * distance;
  // mpu.getAcceleration(&ax, &ay, &az);

  // if(distance < 20.0){
  //   rotateCounterClockwise();
  //   delay(500);
  //   forwardMove();
  // }
  int analogValue = analogRead(A0); // Зчитуємо сигнал з аналогового піну
  Serial.println(analogValue);
  // float voltage = analogValue * (1.0 / 1023.0); // Напруга в вольтах

  if (analogValue >= 12) { // 20 мВ
    Serial.println("Метал детектовано!");
    // Можна активувати якісь дії, наприклад:
    // stopMotors(); // Зупинка моторів
    // delay(1000);
  }


  delay(10); // щоб не спамити сервер
}

void sendTelemetry(String jsonPayload) {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  http.begin(client, "http://example.com");
  http.addHeader("Content-Type", "application/json");

  http.POST(jsonPayload);  // нам неважлива відповідь
  http.end();
}
String getCommand() {
  // Serial.println(0);
  if (WiFi.status() != WL_CONNECTED) return "wifi_error";
  // Serial.println(1);
  HTTPClient http;
  http.begin(client, "http://193.169.189.125:8000/update/get");  // адреса може бути та сама або інша, якщо треба
  // Serial.println(2);
  int httpResponseCode = http.GET();
  // Serial.println(3);

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
void stopMotors() {
  digitalWrite(motorRightPin1, LOW);
  digitalWrite(motorRightPin2, LOW);
  digitalWrite(motorLeftPin1, LOW);
  digitalWrite(motorLeftPin2, LOW);
}

void rotateClockwise() {
  digitalWrite(motorRightPin1, HIGH);
  digitalWrite(motorRightPin2, LOW);
  digitalWrite(motorLeftPin1, HIGH);
  digitalWrite(motorLeftPin2, LOW);
}

void rotateCounterClockwise() {
  digitalWrite(motorRightPin1, LOW);
  digitalWrite(motorRightPin2, HIGH);
  digitalWrite(motorLeftPin1, LOW);
  digitalWrite(motorLeftPin2, HIGH);
}

void backwardMove() {
  digitalWrite(motorRightPin1, LOW);
  digitalWrite(motorRightPin2, HIGH);
  digitalWrite(motorLeftPin1, HIGH);
  digitalWrite(motorLeftPin2, LOW);
}

void forwardMove() {
  digitalWrite(motorRightPin1, HIGH);
  digitalWrite(motorRightPin2, LOW);
  digitalWrite(motorLeftPin1, LOW);
  digitalWrite(motorLeftPin2, HIGH);
}
float distanceUltraSoundSensor(int trigPin, int echoPin) {
  // DISTANCE IN CM
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  float duration = pulseIn(echoPin, HIGH, 8746); // MAX DISTANCE 1 meter
  float distance = (duration*.0343)/2;
  return distance != 0.0 ? distance : 100.0; // Convert to cm
}
