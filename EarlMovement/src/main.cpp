#include <Arduino.h>
// NOTE THE SMALL SIDE IS THE FRONT SIDE

// left side
// ===== Motor pins for first L298N =====

//Right Motor Driver

// --- RIGHT MOTOR DRIVER (L298N #1) ---

// Right Front Motor
int RF_IN1 = 6;
int RF_IN2 = 5;
int RF_ENA = 7;

// Right Back Motor
int RB_IN3 = 3;
int RB_IN4 = 2;
int RB_ENA = 4;


// --- LEFT MOTOR DRIVER (L298N #2) ---

// Left Front Motor
int LF_IN3 = 12;
int LF_IN4 = 11;
int LF_ENA = 13;

// Left Back Motor
int LB_IN1 = 9;
int LB_IN2 = 10;
int LB_ENA = 8;

// Default speed (0–255)
int speed = 255;

// ===================== Helper: Stop all motors =====================
void stopMotors() {
  analogWrite(RF_ENA, 0);
  analogWrite(RB_ENA, 0);
  analogWrite(LF_ENA, 0);
  analogWrite(LB_ENA, 0);

  digitalWrite(RF_IN1, LOW);
  digitalWrite(RF_IN2, LOW);
  digitalWrite(RB_IN3, LOW);
  digitalWrite(RB_IN4, LOW);
  digitalWrite(LF_IN3, LOW);
  digitalWrite(LF_IN4, LOW);
  digitalWrite(LB_IN1, LOW);
  digitalWrite(LB_IN2, LOW);
}

// =====================
// Movement functions (control all 4 motors)
// =====================
// ===================== Move Forward =====================
void forward(int spd) {
  analogWrite(RF_ENA, spd);
  analogWrite(RB_ENA, spd);
  analogWrite(LF_ENA, spd);
  analogWrite(LB_ENA, spd);

  // Right side forward
  digitalWrite(RF_IN1, HIGH);
  digitalWrite(RF_IN2, LOW);
  digitalWrite(RB_IN3, HIGH);
  digitalWrite(RB_IN4, LOW);

  // Left side forward
  digitalWrite(LF_IN3, HIGH);
  digitalWrite(LF_IN4, LOW);
  digitalWrite(LB_IN1, HIGH);
  digitalWrite(LB_IN2, LOW);
}


// ===================== Move Backward =====================
void backward(int spd) {
  analogWrite(RF_ENA, spd);
  analogWrite(RB_ENA, spd);
  analogWrite(LF_ENA, spd);
  analogWrite(LB_ENA, spd);

  // Right side backward
  digitalWrite(RF_IN1, LOW);
  digitalWrite(RF_IN2, HIGH);
  digitalWrite(RB_IN3, LOW);
  digitalWrite(RB_IN4, HIGH);

  // Left side backward
  digitalWrite(LF_IN3, LOW);
  digitalWrite(LF_IN4, HIGH);
  digitalWrite(LB_IN1, LOW);
  digitalWrite(LB_IN2, HIGH);
}


// ===================== Full Turn Left (spin in place) =====================
void fullTurnLeft(int spd) {
  analogWrite(RF_ENA, spd);
  analogWrite(RB_ENA, spd);
  analogWrite(LF_ENA, spd);
  analogWrite(LB_ENA, spd);

  // Right side forward
  digitalWrite(RF_IN1, HIGH);
  digitalWrite(RF_IN2, LOW);
  digitalWrite(RB_IN3, HIGH);
  digitalWrite(RB_IN4, LOW);

  // Left side backward
  digitalWrite(LF_IN3, LOW);
  digitalWrite(LF_IN4, HIGH);
  digitalWrite(LB_IN1, LOW);
  digitalWrite(LB_IN2, HIGH);
}


// ===================== Full Turn Right (spin in place) =====================
void fullTurnRight(int spd) {
  analogWrite(RF_ENA, spd);
  analogWrite(RB_ENA, spd);
  analogWrite(LF_ENA, spd);
  analogWrite(LB_ENA, spd);

  // Left side forward
  digitalWrite(LF_IN3, HIGH);
  digitalWrite(LF_IN4, LOW);
  digitalWrite(LB_IN1, HIGH);
  digitalWrite(LB_IN2, LOW);

  // Right side backward
  digitalWrite(RF_IN1, LOW);
  digitalWrite(RF_IN2, HIGH);
  digitalWrite(RB_IN3, LOW);
  digitalWrite(RB_IN4, HIGH);
}


// ===================== Move Forward + Turn Left (curve left) =====================
void moveAndTurnLeft(int spd) {
  analogWrite(RF_ENA, spd);
  analogWrite(RB_ENA, spd);

  analogWrite(LF_ENA, spd / 2);   // slower left side
  analogWrite(LB_ENA, spd / 2);

  // Forward both sides
  digitalWrite(RF_IN1, HIGH);
  digitalWrite(RF_IN2, LOW);
  digitalWrite(RB_IN3, HIGH);
  digitalWrite(RB_IN4, LOW);

  digitalWrite(LF_IN3, HIGH);
  digitalWrite(LF_IN4, LOW);
  digitalWrite(LB_IN1, HIGH);
  digitalWrite(LB_IN2, LOW);
}


// ===================== Move Forward + Turn Right (curve right) =====================
void moveAndTurnRight(int spd) {
  analogWrite(LF_ENA, spd);
  analogWrite(LB_ENA, spd);

  analogWrite(RF_ENA, spd / 2);   // slower right side
  analogWrite(RB_ENA, spd / 2);

  // Forward both sides
  digitalWrite(RF_IN1, HIGH);
  digitalWrite(RF_IN2, LOW);
  digitalWrite(RB_IN3, HIGH);
  digitalWrite(RB_IN4, LOW);

  digitalWrite(LF_IN3, HIGH);
  digitalWrite(LF_IN4, LOW);
  digitalWrite(LB_IN1, HIGH);
  digitalWrite(LB_IN2, LOW);
}

void setup() {
  // Right Front Motor
  pinMode(RF_IN1, OUTPUT);
  pinMode(RF_IN2, OUTPUT);
  pinMode(RF_ENA, OUTPUT);

  // Right Back Motor
  pinMode(RB_IN3, OUTPUT);
  pinMode(RB_IN4, OUTPUT);
  pinMode(RB_ENA, OUTPUT);

  // Left Front Motor
  pinMode(LF_IN3, OUTPUT);
  pinMode(LF_IN4, OUTPUT);
  pinMode(LF_ENA, OUTPUT);

  // Left Back Motor
  pinMode(LB_IN1, OUTPUT);
  pinMode(LB_IN2, OUTPUT);
  pinMode(LB_ENA, OUTPUT);

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
  delay(3000);
  fullTurnLeft(speed);
  delay(3000);
  }
