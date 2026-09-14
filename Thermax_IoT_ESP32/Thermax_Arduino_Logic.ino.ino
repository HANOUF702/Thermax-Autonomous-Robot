#include "DHT.h"
#include <Servo.h> 
#include <SoftwareSerial.h>

// Communication with ESP32 (Arduino RX: 8, Arduino TX: 9)
SoftwareSerial espSerial(8, 9); 

// ================= Sensor Configurations =================
#define DHTPIN A2       
#define DHTTYPE DHT22   
DHT dht(DHTPIN, DHTTYPE);

const int greenLed = A0; 
const int redLed = A1;   
const int buzzer = 11;   
const int echoPin = 12; 
const int trigPin = 13; 
const int servoPin = 10; 

Servo myServo; 

// ================= Motor Configurations =================
const int ENA = 3;  
const int ENB = 6;  
const int IN1 = 2;  
const int IN2 = 4;
const int IN3 = 5;
const int IN4 = 7;

unsigned long previousMillis = 0; 
const long interval = 2000; 
float currentTemp = 25.0;   
bool dangerMode = false;
bool alertSent = false; 

// ================= Movement & Radar Variables =================
boolean goesForward = false;
int MAX_SPEED = 180; 
int speedSet = 0;
int radarAngle = 90;   
int radarStep = 40;    

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600); // Initialize ESP32 connection
  
  dht.begin();
  myServo.attach(servoPin);
  myServo.write(90); 
  delay(1000); 

  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  Serial.println("--- System Initialized. Starting Thermax IoT ---");
}

void loop() {
  unsigned long currentMillis = millis();

  // --- Environmental Monitoring ---
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    
    if (!isnan(t) && !isnan(h)) {
      currentTemp = t;
    }
  }

  // --- Thermal Alert Logic --- 
  if (currentTemp > 28.0) {
    dangerMode = true;
    if (!alertSent) {
      Serial.print("🚨 ALERT: Critical Temp! Sending to ESP32..."); 
      // Send alert and temp to ESP32 over serial
      espSerial.print("ALERT:");
      espSerial.println(currentTemp);
      alertSent = true; 
    }
    stopRobot(); 
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, LOW);
    digitalWrite(buzzer, HIGH); 
    return; 
  } else {
    dangerMode = false;
    alertSent = false; 
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, HIGH);
    digitalWrite(buzzer, LOW);
  }

  // --- Navigation & 360 Radar ---
  if (!dangerMode) {
    myServo.write(radarAngle);
    delay(40); 
    int distance = getDistance(); 
    
    if(distance <= 35) {
      stopRobot();
      delay(200); 
      moveBackward(); 
      delay(700); 
      stopRobot();
      delay(200);
      
      int distanceF = lookFront();
      delay(200);
      int distanceR = lookRight();
      delay(200);
      int distanceL = lookLeft();
      delay(200);

      if (distanceR >= distanceL && distanceR >= distanceF && distanceR > 25) {
        turnRight(); 
      } 
      else if (distanceL > distanceR && distanceL >= distanceF && distanceL > 25) {
        turnLeft();  
      } 
      else if (distanceF > distanceR && distanceF > distanceL && distanceF > 35) {
        moveForward();
      } 
      else {
        moveBackward(); 
        delay(600); 
        stopRobot();
        delay(200);
        turnRight();
        delay(600); 
      }
      stopRobot();
      delay(150);
      radarAngle = 90; 
    } else {
      moveForward(); 
    }
    
    radarAngle += radarStep;
    if (radarAngle >= 130 || radarAngle <= 50) { 
      radarStep = -radarStep; 
    }
  }
}

int lookFront() { myServo.write(90); delay(400); return getDistance(); }
int lookRight() { myServo.write(20); delay(400); return getDistance(); }
int lookLeft() { myServo.write(160); delay(400); return getDistance(); }

int getDistance() {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000); 
  int cm = duration * 0.034 / 2; 
  if(cm == 0) return 1; 
  return cm;
}

void stopRobot() {
  goesForward = false;
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  analogWrite(ENA, 0); analogWrite(ENB, 0);
} 
  
void moveForward() {
 if(!goesForward) {
    goesForward = true;
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
    for (speedSet = 100; speedSet <= MAX_SPEED; speedSet += 10) {
      analogWrite(ENA, speedSet); analogWrite(ENB, speedSet); delay(5);
    }
  } else {
     analogWrite(ENA, MAX_SPEED); analogWrite(ENB, MAX_SPEED);
  }
}

void moveBackward() {
  goesForward = false;
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  analogWrite(ENA, 255); analogWrite(ENB, 255);
}  

void turnRight() {
  goesForward = false;
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  analogWrite(ENA, 255); analogWrite(ENB, 255);
  delay(600); 
} 
 
void turnLeft() {
  goesForward = false;
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, 255); analogWrite(ENB, 255);
  delay(600);
}