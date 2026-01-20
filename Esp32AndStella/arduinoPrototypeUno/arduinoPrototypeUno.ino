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
      Serial.println(incoming);
      incoming = "";
    } else {
      incoming += c;
    }
  }
}