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
  newDistance = distanceUltraSoundSensor(trigPin, echoPin);
  distance = 0.1 * newDistance + 0.9 * distance;
  Serial.println(distance);
  if(distance > 20.0){
    forwardMove();
    // Serial.println("MOVING!!!");
  }
  else{
    stopMotors();
    // Serial.println("STOPPING!!!");
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