#include <Arduino.h>

// ---------------- RIGHT SIDE (L298N #1) ----------------

// Right Front Motor (OUT1/OUT2)
int RF_IN1 = 6;     
int RF_IN2 = 7;     
int RF_ENA = 5;     

// Right Back Motor (OUT3/OUT4)
int RB_IN3 = 2;     
int RB_IN4 = 4;     
int RB_ENA = 3;     


// ---------------- LEFT SIDE (L298N #2) -----------------

// Left Front Motor (OUT1/OUT2)
int LF_IN1 = 8;     
int LF_IN2 = 13;    
int LF_ENA = 10;    

// Left Back Motor (OUT3/OUT4)
int LB_IN3 = 11;    
int LB_IN4 = 12;    
int LB_ENA = 9;     


int speed = 80;

// for distance reading
String incoming = "";

// Define a struct to hold anchor distances
struct AnchorDistances {
  int anchor1 = 0;
  int anchor2 = 0;
  int anchor3 = 0;
};

AnchorDistances dist;

// ===================== STOP ALL MOTORS =====================
void stopMotors() {
  analogWrite(RF_ENA, 0);
  analogWrite(RB_ENA, 0);
  analogWrite(LF_ENA, 0);
  analogWrite(LB_ENA, 0);

  digitalWrite(RF_IN1, LOW);
  digitalWrite(RF_IN2, LOW);

  digitalWrite(RB_IN3, LOW);
  digitalWrite(RB_IN4, LOW);

  digitalWrite(LF_IN1, LOW);
  digitalWrite(LF_IN2, LOW);

  digitalWrite(LB_IN3, LOW);
  digitalWrite(LB_IN4, LOW);
}


// ===================== FORWARD =====================
void forward(int spd) {
  analogWrite(RF_ENA, spd);
  analogWrite(RB_ENA, spd);
  analogWrite(LF_ENA, spd);
  analogWrite(LB_ENA, spd);

  digitalWrite(RF_IN1, HIGH);
  digitalWrite(RF_IN2, LOW);

  digitalWrite(RB_IN3, HIGH);
  digitalWrite(RB_IN4, LOW);

  digitalWrite(LF_IN1, HIGH);
  digitalWrite(LF_IN2, LOW);

  digitalWrite(LB_IN3, HIGH);
  digitalWrite(LB_IN4, LOW);
}


// ===================== BACKWARD =====================
void backward(int spd) {
  analogWrite(RF_ENA, spd);
  analogWrite(RB_ENA, spd);
  analogWrite(LF_ENA, spd);
  analogWrite(LB_ENA, spd);

  digitalWrite(RF_IN1, LOW);
  digitalWrite(RF_IN2, HIGH);

  digitalWrite(RB_IN3, LOW);
  digitalWrite(RB_IN4, HIGH);

  digitalWrite(LF_IN1, LOW);
  digitalWrite(LF_IN2, HIGH);

  digitalWrite(LB_IN3, LOW);
  digitalWrite(LB_IN4, HIGH);
}


// ===================== FULL TURN LEFT =====================
void fullTurnLeft(int spd) {
  analogWrite(RF_ENA, spd);
  analogWrite(RB_ENA, spd);
  analogWrite(LF_ENA, spd);
  analogWrite(LB_ENA, spd);

  digitalWrite(RF_IN1, HIGH);
  digitalWrite(RF_IN2, LOW);

  digitalWrite(RB_IN3, HIGH);
  digitalWrite(RB_IN4, LOW);

  digitalWrite(LF_IN1, LOW);
  digitalWrite(LF_IN2, HIGH);

  digitalWrite(LB_IN3, LOW);
  digitalWrite(LB_IN4, HIGH);
}


// ===================== FULL TURN RIGHT =====================
void fullTurnRight(int spd) {
  analogWrite(RF_ENA, spd);
  analogWrite(RB_ENA, spd);
  analogWrite(LF_ENA, spd);
  analogWrite(LB_ENA, spd);

  digitalWrite(LF_IN1, HIGH);
  digitalWrite(LF_IN2, LOW);

  digitalWrite(LB_IN3, HIGH);
  digitalWrite(LB_IN4, LOW);

  digitalWrite(RF_IN1, LOW);
  digitalWrite(RF_IN2, HIGH);

  digitalWrite(RB_IN3, LOW);
  digitalWrite(RB_IN4, HIGH);
}


// ===================== CURVE LEFT =====================
void moveAndTurnLeft(int spd) {
  analogWrite(RF_ENA, spd);
  analogWrite(RB_ENA, spd);
  analogWrite(LF_ENA, spd / 2);
  analogWrite(LB_ENA, spd / 2);

  digitalWrite(RF_IN1, HIGH);
  digitalWrite(RF_IN2, LOW);

  digitalWrite(RB_IN3, HIGH);
  digitalWrite(RB_IN4, LOW);

  digitalWrite(LF_IN1, HIGH);
  digitalWrite(LF_IN2, LOW);

  digitalWrite(LB_IN3, HIGH);
  digitalWrite(LB_IN4, LOW);
}


// ===================== CURVE RIGHT =====================
void moveAndTurnRight(int spd) {
  analogWrite(LF_ENA, spd);
  analogWrite(LB_ENA, spd);
  analogWrite(RF_ENA, spd / 2);
  analogWrite(RB_ENA, spd / 2);

  digitalWrite(RF_IN1, HIGH);
  digitalWrite(RF_IN2, LOW);

  digitalWrite(RB_IN3, HIGH);
  digitalWrite(RB_IN4, LOW);

  digitalWrite(LF_IN1, HIGH);
  digitalWrite(LF_IN2, LOW);

  digitalWrite(LB_IN3, HIGH);
  digitalWrite(LB_IN4, LOW);
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


void setup() {
  pinMode(RF_IN1, OUTPUT);
  pinMode(RF_IN2, OUTPUT);
  pinMode(RF_ENA, OUTPUT);

  pinMode(RB_IN3, OUTPUT);
  pinMode(RB_IN4, OUTPUT);
  pinMode(RB_ENA, OUTPUT);

  pinMode(LF_IN1, OUTPUT);
  pinMode(LF_IN2, OUTPUT);
  pinMode(LF_ENA, OUTPUT);

  pinMode(LB_IN3, OUTPUT);
  pinMode(LB_IN4, OUTPUT);
  pinMode(LB_ENA, OUTPUT);

  Serial.begin(9600);
  stopMotors();
  Serial.println("Motors ready!");
}


void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      Serial.print("Received from ESP32: ");
      parseAnchorDistance(incoming);
      Serial.print("Anchor1: "); Serial.println(dist.anchor1);
      Serial.print("Anchor2: "); Serial.println(dist.anchor2);
      Serial.print("Anchor3: "); Serial.println(dist.anchor3);
      calculateDirection();
      incoming = "";
    } else {
      incoming += c;
    }
  }
}