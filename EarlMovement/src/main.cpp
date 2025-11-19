#include <Arduino.h>

// left side
// ===== Motor pins for first L298N =====
int motor1pin1 = 2;
int motor1pin2 = 3;
int motor2pin1 = 4;
int motor2pin2 = 5;
int ENAleftsidebottom = 9;   // Enable for bottom left side
int ENBleftsidetop = 10;  // Enable for top left side

//right side
// ===== Motor pins for second L298N =====
int motor3pin1 = 7;
int motor3pin2 = 8;
int motor4pin1 = 12;
int motor4pin2 = 13;
int ENArightsidetop = 6;   // Enable for top right side
int ENBrightsidebottom = 11;  // Enable for bottom right side

// Default speed (0–255)
int speed = 255;

// =====================
// Helper function: stop all motors
// =====================
void stopMotors() {
  analogWrite(ENAleftsidebottom, 0);
  analogWrite(ENBleftsidetop, 0);
  analogWrite(ENArightsidetop, 0);
  analogWrite(ENBrightsidebottom, 0);

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

  //full speed
  analogWrite(ENAleftsidebottom, spd);
  analogWrite(ENBleftsidetop, spd);
  analogWrite(ENArightsidetop, spd);
  analogWrite(ENBrightsidebottom, spd);
  //Left side foward
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
  //Right side foward
  digitalWrite(motor3pin1, HIGH);
  digitalWrite(motor3pin2, LOW);
  digitalWrite(motor4pin1, HIGH);
  digitalWrite(motor4pin2, LOW);

}

void backward(int spd) {

  //full speed
  analogWrite(ENAleftsidebottom, spd);
  analogWrite(ENBleftsidetop, spd);
  analogWrite(ENArightsidetop, spd);
  analogWrite(ENBrightsidebottom, spd);

  //left side backward
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH);
  // right side backward
  digitalWrite(motor3pin1, LOW);
  digitalWrite(motor3pin2, HIGH);
  digitalWrite(motor4pin1, LOW);
  digitalWrite(motor4pin2, HIGH);

}
// turns left 90 degrees
void fullTurnLeft(int spd) {

  //Full speed for all sides
  analogWrite(ENAleftsidebottom, spd);
  analogWrite(ENBleftsidetop, spd);
  analogWrite(ENArightsidetop, spd);
  analogWrite(ENBrightsidebottom, spd);

  //LEFT backward
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH);
  //Right foward
  digitalWrite(motor3pin1, HIGH);
  digitalWrite(motor3pin2, LOW);
  digitalWrite(motor4pin1, HIGH);
  digitalWrite(motor4pin2, LOW);
  delay(4000);


}

// turns Right 90 degrees
void fullTurnRight(int spd) {

  //Full speed for all sides
  analogWrite(ENAleftsidebottom, spd);
  analogWrite(ENArightsidetop, spd);
  analogWrite(ENBleftsidetop, spd );
  analogWrite(ENBrightsidebottom, spd);

  //LEFT forward
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
  // RIGHT backward
  digitalWrite(motor3pin1, LOW);
  digitalWrite(motor3pin2, HIGH);
  digitalWrite(motor4pin1, LOW);
  digitalWrite(motor4pin2, HIGH);

}
// turns left and moves foward
void moveAndTurnLeft(int spd) {
  analogWrite(ENAleftsidebottom, spd/2);
  analogWrite(ENBleftsidetop, spd/2);
  analogWrite(ENArightsidetop, spd);
  analogWrite(ENBrightsidebottom, spd);
  //Left side foward
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
  //Right side foward
  digitalWrite(motor3pin1, HIGH);
  digitalWrite(motor3pin2, LOW);
  digitalWrite(motor4pin1, HIGH);
  digitalWrite(motor4pin2, LOW);
}
//turns right and moves foward
void moveAndTurnRight(int spd) {
  analogWrite(ENAleftsidebottom, spd);
  analogWrite(ENBleftsidetop, spd);
  analogWrite(ENArightsidetop, spd/2);
  analogWrite(ENBrightsidebottom, spd/2);
  //Left side foward
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
  //Right side foward
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
  pinMode(ENAleftsidebottom, OUTPUT);
  pinMode(ENBleftsidetop, OUTPUT);

  pinMode(motor3pin1, OUTPUT);
  pinMode(motor3pin2, OUTPUT);
  pinMode(motor4pin1, OUTPUT);
  pinMode(motor4pin2, OUTPUT);
  pinMode(ENArightsidetop, OUTPUT);
  pinMode(ENBrightsidebottom, OUTPUT);

  Serial.begin(9600);
  stopMotors();
  Serial.println("Motors ready!");
}

// =====================
// Loop example
// =====================
void loop() {
  Serial.println("Moving forward...");
  fullTurnLeft(speed);
  delay(10000);
  stopMotors();
  delay(10000);
  }
