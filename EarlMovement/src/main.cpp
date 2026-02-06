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
int turnOffset = 30;   // try 20–40 for gentle turns


// for distance reading
String incoming = "";

// Define a struct to hold anchor distances
struct AnchorDistances {
  int anchor1 = 0;
  int anchor2 = 0;
  int anchor3 = 0;
};

AnchorDistances dist;

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

void moveAndTurnLeft2(int spd) {
  int leftSpeed = spd - turnOffset;
  if (leftSpeed < 0) leftSpeed = 0;

  analogWrite(RF_ENA, spd);
  analogWrite(RB_ENA, spd);

  analogWrite(LF_ENA, leftSpeed);
  analogWrite(LB_ENA, leftSpeed);

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


void moveAndTurnRight2(int spd) {
  int rightSpeed = spd - turnOffset;
  if (rightSpeed < 0) rightSpeed = 0;

  analogWrite(LF_ENA, spd);
  analogWrite(LB_ENA, spd);

  analogWrite(RF_ENA, rightSpeed);
  analogWrite(RB_ENA, rightSpeed);

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

// ===================== SERIAL PARSING =====================
void parseAnchorDistance(String line){


  // Step 1: Find the anchor number 
  int anchorID = 0;
  int anchorPos = line.indexOf("Anchor ");
  int dashPos   = line.indexOf(" -");
  if (anchorPos != -1 && dashPos != -1) {
      String idStr = line.substring(anchorPos + 7, dashPos);
      anchorID = idStr.toInt(); // 1, 2, or 3
  }

  // Step 2: Find the Distance Value
  int colonIndex = line.indexOf(": ");
  int distance = 0;
  if (colonIndex != -1) {
      String distanceStr = line.substring(colonIndex + 2);
      int spaceIndex = distanceStr.indexOf(" ");
      if (spaceIndex != -1) {
          distanceStr = distanceStr.substring(0, spaceIndex);
      }
      distance = distanceStr.toInt();
  }

  //Step 3: Assign to correct anchor variable
  switch(anchorID) {
      case 1: dist.anchor1 = distance; break;
      case 2: dist.anchor2 = distance; break;
      case 3: dist.anchor3 = distance; break;
  }
}

void calculateDirection(){
  if (dist.anchor1 == -1|| dist.anchor1 == 65535 || dist.anchor2 == -1 || dist.anchor2 == 65535){
    // checking if tag is disconnected
    Serial.println("Stop Moving - check if tag is disconnected");
    stopMotors();
    return;
  }
  // distance of either 1 and 2 is greater than 10mm
  // continue going
  if ((dist.anchor1 >30) && (dist.anchor2>30)){
      if (dist.anchor1 < 250 || dist.anchor2 < 250) {
      // stopping motor to little distance
      Serial.println("Stopping Motors");
      stopMotors();
      }
    else {
      if (abs(dist.anchor1 - dist.anchor2) < 15){
      // Move Forward
      Serial.println("Moving Forward");
      forward(speed);
      }
      else {
        if (dist.anchor1>dist.anchor2){
          // Turn Right
          Serial.println("Turning Right");
          moveAndTurnRight2(speed);
        }
        else {
          // Turn Left
          Serial.println("Turning Left");
          moveAndTurnLeft2(speed);
        }
      }
    }
  }
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

  Serial.begin(115200);
  Serial1.begin(9600);

  stopMotors();
  Serial.println("Motors ready!");
}



// =====================
// Loop example
// =====================
void loop() {
  // Check if any data has arrived from the ESP32
  if (Serial1.available()) {
    // Read the line until the newline character '\n'
    String incomingLine = Serial1.readStringUntil('\n');
    
    // Print the raw string exactly as it arrived
    Serial.print("Received from ESP32: ");
    Serial.println(incomingLine);
    parseAnchorDistance(incomingLine);
    Serial.print("Anchor1: "); Serial.println(dist.anchor1);
    Serial.print("Anchor2: "); Serial.println(dist.anchor2);
  //   Serial.print("Anchor3: "); Serial.println(dist.anchor3);
    calculateDirection();

  }
}
  