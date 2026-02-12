#include <Arduino.h>

// Left Front Motor
int L_IN1 = 6;
int L_IN2 = 5;
int L_ENA = 7;

// Right Front Motor
int R_IN1 = 8;
int R_IN2 = 9;
int R_ENA = 10;

// Default speed (0–255)
int speed = 255;
// int turnOffset = 80;   // try 20–40 for gentle turns


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
  analogWrite(L_ENA, 0);
  analogWrite(R_ENA, 0);

  digitalWrite(R_IN1, LOW);
  digitalWrite(R_IN2, LOW);
  digitalWrite(L_IN1, LOW);
  digitalWrite(L_IN2, LOW);
}

// =====================
// Movement functions (control all 4 motors)
// =====================
// ===================== Move Forward =====================
void forward(int spd) {
  analogWrite(R_ENA, spd);
  analogWrite(L_ENA, spd);

  // Right side forward
  digitalWrite(R_IN1, HIGH);
  digitalWrite(R_IN2, LOW);

  // Left side forward
  digitalWrite(L_IN1, HIGH);
  digitalWrite(L_IN2, LOW);
}


// ===================== Move Backward =====================
void backward(int spd) {
  analogWrite(R_ENA, spd);
  analogWrite(L_ENA, spd);


  // Right side backward
  digitalWrite(R_IN1, LOW);
  digitalWrite(R_IN2, HIGH);

  // Left side backward
  digitalWrite(L_IN1, LOW);
  digitalWrite(L_IN2, HIGH);
}



void moveAndTurnLeft2(int spd, int percentSpd) {
  // int leftSpeed = spd - turnOffset;
  // if (leftSpeed < 0) leftSpeed = 0;

  analogWrite(R_ENA, spd);
  analogWrite(L_ENA, spd * (percentSpd / 100.0));

  // Forward both sides
  digitalWrite(R_IN1, HIGH);
  digitalWrite(R_IN2, LOW);
  digitalWrite(L_IN1, HIGH);
  digitalWrite(L_IN2, LOW);
}


void moveAndTurnRight2(int spd, int percentSpd) {
  // int rightSpeed = spd - turnOffset;
  // if (rightSpeed < 0) rightSpeed = 0;

  analogWrite(L_ENA, spd);
  analogWrite(R_ENA, spd * (percentSpd / 100.0));
  // analogWrite(R_ENA, rightSpeed);

  // Forward both sides
  digitalWrite(R_IN1, HIGH);
  digitalWrite(R_IN2, LOW);
  digitalWrite(L_IN1, HIGH);
  digitalWrite(L_IN2, LOW);
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
void calculateDirection() {

  // =============================
  // 1️⃣ Check if tag disconnected
  // =============================
  if (dist.anchor1 == -1 || dist.anchor1 == 65535 ||
      dist.anchor2 == -1 || dist.anchor2 == 65535 ||
      dist.anchor3 == -1 || dist.anchor3 == 65535) {

    Serial.println("Stop Moving - tag disconnected");
    stopMotors();
    return;
  }

  // =============================
  // 2️⃣ Make sure anchors valid
  // =============================
  if (dist.anchor1 <= 30 || dist.anchor2 <= 30) {
    Serial.println("Invalid small reading");
    stopMotors();
    return;
  }

  // =============================
  // 3️⃣ Stop if too close
  // =============================
  if (dist.anchor1 < 175 || dist.anchor2 < 175) {
    Serial.println("Stopping Motors - too close");
    stopMotors();
    return;
  }

  // =============================
  // 4️⃣ Wrong direction check
  // =============================
  if (dist.anchor3 > dist.anchor1 && dist.anchor3 > dist.anchor2) {

    Serial.println("Wrong direction - pivot");

    if (dist.anchor1 > dist.anchor2) {
      Serial.println("Pivot Right");
      moveAndTurnRight2(speed, 0);
    } else {
      Serial.println("Pivot Left");
      moveAndTurnLeft2(speed, 0);
    }

    return;
  }

  // =============================
  // 5️⃣ Normal steering logic
  // =============================

  int diff = abs(dist.anchor1 - dist.anchor2);

  if (diff < 15) {
    Serial.println("Moving Forward");
    forward(speed);
  }
  else if (diff < 40) {
    Serial.println("Moderate Turn");

    if (dist.anchor1 > dist.anchor2) {
      moveAndTurnRight2(speed, 70);
    } else {
      moveAndTurnLeft2(speed, 70);
    }
  }
  else {
    Serial.println("Sharp Turn");

    if (dist.anchor1 > dist.anchor2) {
      moveAndTurnRight2(speed, 30);
    } else {
      moveAndTurnLeft2(speed, 30);
    }
  }
}


void setup() {
  // Right Front Motor
  pinMode(R_IN1, OUTPUT);
  pinMode(R_IN2, OUTPUT);
  pinMode(R_ENA, OUTPUT);

  // Left Back Motor
  pinMode(L_IN1, OUTPUT);
  pinMode(L_IN2, OUTPUT);
  pinMode(L_ENA, OUTPUT);

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
    // Serial.print("Received from ESP32: ");
    // Serial.println(incomingLine);
    parseAnchorDistance(incomingLine);
    Serial.print("Anchor1: "); Serial.println(dist.anchor1);
    Serial.print("Anchor2: "); Serial.println(dist.anchor2);
    Serial.print("Anchor3: "); Serial.println(dist.anchor3);
    calculateDirection();

  }
}
  