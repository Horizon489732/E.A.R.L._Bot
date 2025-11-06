#include <Arduino.h>

// ===== Motor pins for first L298N =====
int motor1pin1 = 2;
int motor1pin2 = 3;
int motor2pin1 = 4;
int motor2pin2 = 5;
int ENA = 9;   // Enable for left side
int ENB = 10;  // Enable for right side

// ===== Motor pins for second L298N =====
int motor3pin1 = 7;
int motor3pin2 = 8;
int motor4pin1 = 12;
int motor4pin2 = 13;
int ENA2 = 6;   // Enable for second left side
int ENB2 = 11;  // Enable for second right side

// Default speed (0–255)
int speed = 255;

// =====================
// Helper function: stop all motors
// =====================
void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  analogWrite(ENA2, 0);
  analogWrite(ENB2, 0);

  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, LOW);
  digitalWrite(motor3pin1, LOW);
  digitalWrite(motor3pin2, LOW);
  digitalWrite(motor4pin1, LOW);
  digitalWrite(motor4pin2, LOW);
}

// =====================
// Movement functions (control all 4 motors)
// =====================
void forward(int spd) {
  analogWrite(ENA, spd);
  analogWrite(ENB, spd);
  analogWrite(ENA2, spd);
  analogWrite(ENB2, spd);

  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
  digitalWrite(motor3pin1, HIGH);
  digitalWrite(motor3pin2, LOW);
  digitalWrite(motor4pin1, HIGH);
  digitalWrite(motor4pin2, LOW);
}

void backward(int spd) {
  analogWrite(ENA, spd);
  analogWrite(ENB, spd);
  analogWrite(ENA2, spd);
  analogWrite(ENB2, spd);

  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH);
  digitalWrite(motor3pin1, LOW);
  digitalWrite(motor3pin2, HIGH);
  digitalWrite(motor4pin1, LOW);
  digitalWrite(motor4pin2, HIGH);
}

void turnLeft(int spd) {
  // Left side slower
  analogWrite(ENA, spd / 2);
  analogWrite(ENA2, spd / 2);
  analogWrite(ENB, spd);
  analogWrite(ENB2, spd);

  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
  digitalWrite(motor3pin1, HIGH);
  digitalWrite(motor3pin2, LOW);
  digitalWrite(motor4pin1, HIGH);
  digitalWrite(motor4pin2, LOW);
}

void turnRight(int spd) {
  // Right side slower
  analogWrite(ENA, spd);
  analogWrite(ENA2, spd);
  analogWrite(ENB, spd / 2);
  analogWrite(ENB2, spd / 2);

  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
  digitalWrite(motor3pin1, HIGH);
  digitalWrite(motor3pin2, LOW);
  digitalWrite(motor4pin1, HIGH);
  digitalWrite(motor4pin2, LOW);
}

// =====================
// Setup
// =====================
void setup() {
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(motor3pin1, OUTPUT);
  pinMode(motor3pin2, OUTPUT);
  pinMode(motor4pin1, OUTPUT);
  pinMode(motor4pin2, OUTPUT);
  pinMode(ENA2, OUTPUT);
  pinMode(ENB2, OUTPUT);

  Serial.begin(9600);
  stopMotors();
  Serial.println("Motors ready!");
}

// =====================
// Loop example
// =====================
void loop() {
  Serial.println("Moving forward...");
  forward(speed);
  delay(2000);
}
