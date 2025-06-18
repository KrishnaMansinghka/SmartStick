#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include <Adafruit_VEML6075.h>
#include <Servo.h>

// LiDAR sensor and UV sensor
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
Adafruit_VEML6075 uv = Adafruit_VEML6075();

// Servo to sweep LiDAR horizontally
Servo scanner;
const int SERVO_PIN = 9;
const int SERVO_MIN_ANGLE = 0;
const int SERVO_MAX_ANGLE = 180;
const int SERVO_STEP = 15;      // scanning resolution
const int LIDAR_SAMPLES = 3;    // average N readings per angle

// IR sensor pins
const int IR_LEFT_PIN = A0;
const int IR_RIGHT_PIN = A1;

// Buzzer and vibration motor pins
const int BUZZER_PIN = 8;
const int VIBRATION_PIN = 7;

// Thresholds for obstacle detection
const uint16_t LIDAR_THRESHOLD = 800;  // 80 cm
const int IR_THRESHOLD = 400;          // adjust based on sensor output

int irLeftBaseline = 0;
int irRightBaseline = 0;

void setup() {
  Serial.begin(9600);
  pinMode(IR_LEFT_PIN, INPUT);
  pinMode(IR_RIGHT_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(VIBRATION_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(VIBRATION_PIN, LOW);
  scanner.attach(SERVO_PIN);

  if(!lox.begin()) {
    Serial.println("Failed to boot VL53L0X");
    while(1);
  }
  if(!uv.begin()) {
    Serial.println("Failed to find VEML6075 sensor");
    while(1);
  }

  // Calibrate IR baselines
  for(int i=0;i<10;i++) {
    irLeftBaseline += analogRead(IR_LEFT_PIN);
    irRightBaseline += analogRead(IR_RIGHT_PIN);
    delay(20);
  }
  irLeftBaseline /= 10;
  irRightBaseline /= 10;
}

void scanAndReport() {
  for(int angle = SERVO_MIN_ANGLE; angle <= SERVO_MAX_ANGLE; angle += SERVO_STEP) {
    scanner.write(angle);
    delay(200); // allow servo to reach position

    uint32_t distanceSum = 0;
    for(int i=0; i<LIDAR_SAMPLES; i++) {
      VL53L0X_RangingMeasurementData_t measure;
      lox.rangingTest(&measure, false);
      if(measure.RangeStatus == 0) {
        distanceSum += measure.RangeMilliMeter;
      } else {
        distanceSum += LIDAR_THRESHOLD + 1; // treat as too far
      }
      delay(50);
    }
    uint16_t avgDist = distanceSum / LIDAR_SAMPLES;

    int irLeft = analogRead(IR_LEFT_PIN) - irLeftBaseline;
    int irRight = analogRead(IR_RIGHT_PIN) - irRightBaseline;

    float uva = uv.readUVA();
    float uvb = uv.readUVB();
    float uvIndex = uv.readUVI();

    bool obstacle = false;
    if(avgDist < LIDAR_THRESHOLD) obstacle = true;
    if(irLeft > IR_THRESHOLD || irRight > IR_THRESHOLD) obstacle = true;

    if(obstacle) {
      tone(BUZZER_PIN, 2000, 100); // short beep
      digitalWrite(VIBRATION_PIN, HIGH);
    } else {
      noTone(BUZZER_PIN);
      digitalWrite(VIBRATION_PIN, LOW);
    }

    Serial.print("Angle:"); Serial.print(angle);
    Serial.print(",Dist:"); Serial.print(avgDist);
    Serial.print(",IR_L:"); Serial.print(irLeft);
    Serial.print(",IR_R:"); Serial.print(irRight);
    Serial.print(",UVA:"); Serial.print(uva, 2);
    Serial.print(",UVB:"); Serial.print(uvb, 2);
    Serial.print(",UVI:"); Serial.print(uvIndex, 2);
    Serial.print(",Obs:"); Serial.println(obstacle ? "1" : "0");
  }
}

void loop() {
  scanAndReport();
}
