#include <MPU6050.h>


// PINS
const int echoPin = 8;
const int trigPin = 9;
int motorRightPin1 = 2;                                                 
int motorRightPin2 = 3;
int motorLeftPin1 = 4;
int motorLeftPin2 = 5;

// VARIABLES
float distance = 100.0;
float newDistance;

// ACCELEROMETER
MPU6050 mpu;
int16_t ax, ay, az;
// mpu.getAcceleration(&ax, &ay, &az);

// COUNTER
int counter = 0;

// METAL DETECTOR
float metalDetect;

void setup() {

  // ULTRASOUND SENSOR
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // MOTORS
  pinMode(motorRightPin1, OUTPUT);
  pinMode(motorRightPin2, OUTPUT);
  pinMode(motorLeftPin1, OUTPUT);
  pinMode(motorLeftPin2, OUTPUT);

  // INITIALIZE SERIAL PORT
  Serial.begin(9600);

  // INITIALIZE ACCELEROMETER
  mpu.initialize();
  // if (mpu.testConnection()) {
  //   Serial.println("MPU6050 з'єднано!");
  // } else {
  //   Serial.println("Помилка з'єднання з MPU6050");
  // }
}
void loop() {
  // distance = distanceUltraSoundSensor(trigPin, echoPin);
  // Serial.print("Distance: ");
  // Serial.println(distance);
  // delay(500);
  // rotateClockwise();
  // delay(2000);
  // rotateCounterClockwise();
  // delay(2000);
  // forwardMove();
  // delay(2000);
  // backwardMove();
  // delay(2000);
  // stopMotors();
  // delay(500);
  // newDistance = distanceUltraSoundSensor(trigPin, echoPin);
  // distance = 0.1 * newDistance + 0.9 * distance;
  // Serial.println(distance);
  // if(distance > 20.0){
  //   forwardMove();
  //   // Serial.println("MOVING!!!");
  // }
  // else{
  //   stopMotors();
  //   // Serial.println("STOPPING!!!");
  // }
  // mpu.getAcceleration(&ax, &ay, &az);
  // //Serial.print(counter); counter++; Serial.print(" ");
  // Serial.print(" "); Serial.print(ax);
  // Serial.print(" "); Serial.print(ay);
  // Serial.print(" "); Serial.println(az);
  // delay(500);

  // GET TELEMETRY
  newDistance = distanceUltraSoundSensor(trigPin, echoPin);
  distance = 0.1 * newDistance + 0.9 * distance;
  mpu.getAcceleration(&ax, &ay, &az);
  metalDetect = 100.0;
  // 2. Формуємо JSON

  StaticJsonDocument<200> doc;
  doc["ax"] = ax;
  doc["ay"] = ay;
  doc["az"] = az;
  doc["distance"] = distance;
  doc["metal_detect"] = metalDetect;

  String json;
  serializeJson(doc, json);

  // 3. Надсилаємо ESP через UART
  Serial.println(json);

  // 4. Чекаємо відповідь-команду
  unsigned long start = millis();
  while (Serial.available() == 0 && millis() - start < 1000); // максимум 1 сек

  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    // Тут можна виконати команду
    if (command == "rotateLeft") {
      // повертаємо ліворуч
    } else if (command == "moveForward") {
      // їдемо вперед
    }
    // і так далі
  }

  delay(500); // щоб не спамити

  // EXECUTE SERVER COMMANDS
  if (cmd == "rotateLeft") rotateCounterClockwise();
  else if (cmd == "rotateRight") rotateClockwise();
  else if (cmd == "moveForward") forwardMove();
  else if (cmd == "moveBackward") backwardMove();
  else stopMotors();
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
String sendTelemetry(float ax, float ay, float az, float distance, float metalDetect) {
  String json = "{";
  json += "\"ax\":" + String(ax) + ",";
  json += "\"ay\":" + String(ay) + ",";
  json += "\"az\":" + String(az) + ",";
  json += "\"distance\":" + String(distance, 1) + ",";
  json += "\"metal_detect\":" + String(metalDetect, 1);
  json += "}";

  Serial.println(json);  // Надсилаємо JSON на ESP

  unsigned long startTime = millis();
  while (!Serial.available()) {
    if (millis() - startTime > 3000) return "timeout"; // максимум 3 сек очікування
  }

  String response = Serial.readStringUntil('\n');
  response.trim();

  // Припускаємо, що ESP повертає JSON {"command": "rotateLeft"}
  int startIdx = response.indexOf(":\"") + 2;
  int endIdx = response.indexOf("\"", startIdx);
  if (startIdx > 1 && endIdx > startIdx) {
    return response.substring(startIdx, endIdx);
  } else {
    return "error";
  }
}
