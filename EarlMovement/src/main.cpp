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

// for distance reading
String incoming = "";

// ===================== HC-SR04 SETUP =====================
// Current sensors (you already have these)
const int L_TRIG_PIN = 11;   // LEFT FRONT
const int L_ECHO_PIN = 12;

const int R_TRIG_PIN = 2;    // RIGHT FRONT
const int R_ECHO_PIN = 3;

// NEW sensors (CHANGE THESE PINS TO MATCH YOUR WIRING)
const int L2_TRIG_PIN = 47;   // LEFT SIDE (new)
const int L2_ECHO_PIN = 46;

const int R2_TRIG_PIN = 45;  // RIGHT SIDE (new)  <-- change if you want
const int R2_ECHO_PIN = 44;  // RIGHT SIDE (new)  <-- change if you want

float leftTiming = 0.0;
float leftDistanceCm = 0.0;
float rightTiming = 0.0;
float rightDistanceCm = 0.0;

float left2Timing = 0.0;
float left2DistanceCm = 0.0;
float right2Timing = 0.0;
float right2DistanceCm = 0.0;

// Anchor distances
struct AnchorDistances {
  int anchor1 = 0;
  int anchor2 = 0;
  int anchor3 = 0;
};

AnchorDistances dist;

// ===================== Avoidance settings/state =====================
const int AVOID_CM = 30;        // object avoidance threshold
const int DOORWAY_CM = 20;      // doorway override threshold

int avoidSide = 0;              // 0 = none, 1 = avoiding LEFT side, 2 = avoiding RIGHT side
int avoidSensor = 0;            // 1 = front sensor triggered, 2 = side sensor triggered
bool avoidPrinted = false;      // print message once when starting avoidance

// ===================== Helper: Stop all motors =====================
void stopMotors() {
  analogWrite(L_ENA, 0);
  analogWrite(R_ENA, 0);

  digitalWrite(R_IN1, LOW);
  digitalWrite(R_IN2, LOW);
  digitalWrite(L_IN1, LOW);
  digitalWrite(L_IN2, LOW);
}

// ===================== Move Forward =====================
void forward(int spd) {
  analogWrite(R_ENA, spd * 0.8);
  analogWrite(L_ENA, spd );

  digitalWrite(R_IN1, HIGH);
  digitalWrite(R_IN2, LOW);

  digitalWrite(L_IN1, HIGH);
  digitalWrite(L_IN2, LOW);
}

// ===================== Move Backward =====================
void backward(int spd) {
  analogWrite(R_ENA, spd);
  analogWrite(L_ENA, spd);

  digitalWrite(R_IN1, LOW);
  digitalWrite(R_IN2, HIGH);

  digitalWrite(L_IN1, LOW);
  digitalWrite(L_IN2, HIGH);
}

void moveAndTurnLeft2(int spd, int percentSpd) {
  analogWrite(R_ENA, spd);
  analogWrite(L_ENA, spd * (percentSpd / 100.0));

  digitalWrite(R_IN1, HIGH);
  digitalWrite(R_IN2, LOW);
  digitalWrite(L_IN1, HIGH);
  digitalWrite(L_IN2, LOW);
}

void moveAndTurnRight2(int spd, int percentSpd) {
  analogWrite(L_ENA, spd);
  analogWrite(R_ENA, spd * (percentSpd / 100.0));

  digitalWrite(R_IN1, HIGH);
  digitalWrite(R_IN2, LOW);
  digitalWrite(L_IN1, HIGH);
  digitalWrite(L_IN2, LOW);
}

// ===================== SERIAL PARSING =====================
void parseAnchorDistance(String line){
  int anchorID = 0;
  int anchorPos = line.indexOf("Anchor ");
  int dashPos   = line.indexOf(" -");
  if (anchorPos != -1 && dashPos != -1) {
      String idStr = line.substring(anchorPos + 7, dashPos);
      anchorID = idStr.toInt();
  }

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

  switch(anchorID) {
      case 1: dist.anchor1 = distance; break;
      case 2: dist.anchor2 = distance; break;
      case 3: dist.anchor3 = distance; break;
  }
}

// ===================== Ultrasonic reads =====================

// LEFT FRONT
float readLeftUltrasonicCm() {
  digitalWrite(L_TRIG_PIN, LOW);
  delayMicroseconds(5);

  digitalWrite(L_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(L_TRIG_PIN, LOW);

  leftTiming = pulseIn(L_ECHO_PIN, HIGH, 30000);
  if (leftTiming == 0) return 0;
  return (leftTiming * 0.0344) / 2.0;
}

// RIGHT FRONT
float readRightUltrasonicCm() {
  digitalWrite(R_TRIG_PIN, LOW);
  delayMicroseconds(5);

  digitalWrite(R_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(R_TRIG_PIN, LOW);

  rightTiming = pulseIn(R_ECHO_PIN, HIGH, 30000);
  if (rightTiming == 0) return 0;
  return (rightTiming * 0.0344) / 2.0;
}

// LEFT SIDE (new)
float readLeft2UltrasonicCm() {
  digitalWrite(L2_TRIG_PIN, LOW);
  delayMicroseconds(5);

  digitalWrite(L2_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(L2_TRIG_PIN, LOW);

  left2Timing = pulseIn(L2_ECHO_PIN, HIGH, 30000);
  if (left2Timing == 0) return 0;
  return (left2Timing * 0.0344) / 2.0;
}

// RIGHT SIDE (new)
float readRight2UltrasonicCm() {
  digitalWrite(R2_TRIG_PIN, LOW);
  delayMicroseconds(5);

  digitalWrite(R2_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(R2_TRIG_PIN, LOW);

  right2Timing = pulseIn(R2_ECHO_PIN, HIGH, 30000);
  if (right2Timing == 0) return 0;
  return (right2Timing * 0.0344) / 2.0;
}

void calculateDirection() {

  // =============================
  // If we're already avoiding a side,
  // keep turning until BOTH sensors on that side are clear.
  // While doing this, ignore everything else.
  // =============================
  if (avoidSide == 1) { // avoiding LEFT side -> turn RIGHT
    bool leftBlocked = (leftDistanceCm > 0 && leftDistanceCm <= AVOID_CM);
    bool left2Blocked = (left2DistanceCm > 0 && left2DistanceCm <= AVOID_CM);

    if (leftBlocked || left2Blocked) {
      if (!avoidPrinted) {
        Serial.print("Avoiding LEFT side (trigger sensor ");
        Serial.print(avoidSensor);
        Serial.println(") -> turning RIGHT until clear");
        avoidPrinted = true;
      }
      moveAndTurnRight2(speed, 20);
      return;
    } else {
      Serial.println("LEFT side clear -> stopping avoidance");
      avoidSide = 0;
      avoidSensor = 0;
      avoidPrinted = false;
      // fall through to normal logic
    }
  }

  if (avoidSide == 2) { // avoiding RIGHT side -> turn LEFT
    bool rightBlocked = (rightDistanceCm > 0 && rightDistanceCm <= AVOID_CM);
    bool right2Blocked = (right2DistanceCm > 0 && right2DistanceCm <= AVOID_CM);

    if (rightBlocked || right2Blocked) {
      if (!avoidPrinted) {
        Serial.print("Avoiding RIGHT side (trigger sensor ");
        Serial.print(avoidSensor);
        Serial.println(") -> turning LEFT until clear");
        avoidPrinted = true;
      }
      moveAndTurnLeft2(speed, 20);
      return;
    } else {
      Serial.println("RIGHT side clear -> stopping avoidance");
      avoidSide = 0;
      avoidSensor = 0;
      avoidPrinted = false;
      // fall through to normal logic
    }
  }

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
  // Doorway balancing override (front sensors only)
  // =============================
  if (leftDistanceCm > 0 && rightDistanceCm > 0 &&
      leftDistanceCm <= DOORWAY_CM && rightDistanceCm <= DOORWAY_CM) {

    Serial.println("Doorway balancing: BOTH <= 20cm -> going straight");
    forward(speed);
    return;
  }

  // =============================
  // NEW: Side avoidance start
  // If ONE sensor on a side sees something, start turning away.
  // Ignore the other sensor for "choosing" what to do (no flip-flopping),
  // but require BOTH sensors on that side to clear to stop turning.
  // =============================

  // LEFT side triggers -> start avoiding LEFT side (turn RIGHT)
  if (leftDistanceCm > 0 && leftDistanceCm <= AVOID_CM) {
    avoidSide = 1;
    avoidSensor = 1;   // front left triggered
    avoidPrinted = false;
    moveAndTurnRight2(speed, 20);
    return;
  }
  if (left2DistanceCm > 0 && left2DistanceCm <= AVOID_CM) {
    avoidSide = 1;
    avoidSensor = 2;   // side left triggered
    avoidPrinted = false;
    moveAndTurnRight2(speed, 20);
    return;
  }

  // RIGHT side triggers -> start avoiding RIGHT side (turn LEFT)
  if (rightDistanceCm > 0 && rightDistanceCm <= AVOID_CM) {
    avoidSide = 2;
    avoidSensor = 1;   // front right triggered
    avoidPrinted = false;
    moveAndTurnLeft2(speed, 20);
    return;
  }
  if (right2DistanceCm > 0 && right2DistanceCm <= AVOID_CM) {
    avoidSide = 2;
    avoidSensor = 2;   // side right triggered
    avoidPrinted = false;
    moveAndTurnLeft2(speed, 20);
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
      moveAndTurnRight2(speed, 40);
    } else {
      moveAndTurnLeft2(speed, 70);
    }
  }
  else {
    Serial.println("Sharp Turn");

    if (dist.anchor1 > dist.anchor2) {
      moveAndTurnRight2(speed, 15);
    } else {
      moveAndTurnLeft2(speed, 30);
    }
  }
}

void setup() {
  pinMode(R_IN1, OUTPUT);
  pinMode(R_IN2, OUTPUT);
  pinMode(R_ENA, OUTPUT);

  pinMode(L_IN1, OUTPUT);
  pinMode(L_IN2, OUTPUT);
  pinMode(L_ENA, OUTPUT);

  // LEFT FRONT
  pinMode(L_TRIG_PIN, OUTPUT);
  pinMode(L_ECHO_PIN, INPUT);
  digitalWrite(L_TRIG_PIN, LOW);

  // RIGHT FRONT
  pinMode(R_TRIG_PIN, OUTPUT);
  pinMode(R_ECHO_PIN, INPUT);
  digitalWrite(R_TRIG_PIN, LOW);

  // LEFT SIDE (new)
  pinMode(L2_TRIG_PIN, OUTPUT);
  pinMode(L2_ECHO_PIN, INPUT);
  digitalWrite(L2_TRIG_PIN, LOW);

  // RIGHT SIDE (new)
  pinMode(R2_TRIG_PIN, OUTPUT);
  pinMode(R2_ECHO_PIN, INPUT);
  digitalWrite(R2_TRIG_PIN, LOW);

  Serial.begin(115200);
  Serial1.begin(9600);

  stopMotors();
  Serial.println("Motors ready!");
}

void loop() {
  // Read all ultrasonic distances
  leftDistanceCm = readLeftUltrasonicCm();
  rightDistanceCm = readRightUltrasonicCm();
  left2DistanceCm = readLeft2UltrasonicCm();
  right2DistanceCm = readRight2UltrasonicCm();

  // prints <= 50 cm
  if (rightDistanceCm > 0 && rightDistanceCm <= 50) {
    Serial.print("Right Front HC-SR04: ");
    Serial.print(rightDistanceCm);
    Serial.println(" cm");
  }
  if (leftDistanceCm > 0 && leftDistanceCm <= 50) {
    Serial.print("Left Front HC-SR04: ");
    Serial.print(leftDistanceCm);
    Serial.println(" cm");
  }
  if (left2DistanceCm > 0 && left2DistanceCm <= 50) {
    Serial.print("Left Side HC-SR04: ");
    Serial.print(left2DistanceCm);
    Serial.println(" cm");
  }
  if (right2DistanceCm > 0 && right2DistanceCm <= 50) {
    Serial.print("Right Side HC-SR04: ");
    Serial.print(right2DistanceCm);
    Serial.println(" cm");
  }

  // If any data has arrived from the ESP32
  if (Serial1.available()) {
    String incomingLine = Serial1.readStringUntil('\n');

    parseAnchorDistance(incomingLine);
    Serial.print("Anchor1: "); Serial.println(dist.anchor1);
    Serial.print("Anchor2: "); Serial.println(dist.anchor2);
    Serial.print("Anchor3: "); Serial.println(dist.anchor3);

    calculateDirection();
  } else {
    // even without anchor updates, keep avoidance responsive
    if (avoidSide != 0) {
      calculateDirection();
    }
  }

  delay(20);
}