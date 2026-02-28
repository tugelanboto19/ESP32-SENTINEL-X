#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_VL53L0X.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "time.h"

// --- KONFIGURASI PIN & HARDWARE ---
#define SDA_PIN 19
#define SCL_PIN 22
#define SERVO_PIN 26
int SERVO_OFFSET = -40; 

Adafruit_SSD1306 display(128, 64, &Wire, -1);
Adafruit_BMP085 bmp;
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
Servo myServo;

// --- FSM (FINITE STATE MACHINE) ---
enum State { SCANNING, LOCKING, LOCKED_WAIT };
State currentState = SCANNING;

unsigned long timerInterval = 0;
int currentAngle = 0;
int bestAngle = 90;
int minDistance = 2000;
bool isMovingState = true; 
unsigned long lockStartMillis = 0;
unsigned long nextBlink = 0;
bool isBlinking = false;

const long LOCK_DURATION = 180000; 
const long SCAN_STEP_TIME = 1000;  

const char* ssid = "Annasya"; // Tetap pakai WiFi Mas Imam
const char* password = "tanyanasya";
String nextPrayerDisplay = "";

struct PrayerTimes {
  int hours[5], mins[5];
  String names[5] = {"SUBUH", "DZUHUR", "ASHAR", "MAGHRIB", "ISYA"};
} schedule;

// --- UI LOGIC ---
void drawWiFiBar() {
  if (WiFi.status() == WL_CONNECTED) {
    int bars = map(constrain(WiFi.RSSI(), -100, -50), -100, -50, 1, 4);
    for (int i = 0; i < bars; i++) display.fillRect(i * 3, 7 - ((i + 1) * 2), 2, (i + 1) * 2, WHITE);
  }
}

void drawHeader(float temp) {
  display.setTextSize(1);
  drawWiFiBar();
  display.setCursor(35, 0); display.print(nextPrayerDisplay);
  display.setCursor(110, 0); display.printf("%.0fC", temp);
}

void drawLargeEyes(bool open) {
  if (open) {
    display.fillRect(25, 12, 30, 25, WHITE);
    display.fillRect(73, 12, 30, 25, WHITE);
  } else {
    display.fillRect(25, 22, 30, 5, WHITE); 
    display.fillRect(73, 22, 30, 5, WHITE);
  }
}

// --- CORE LOGIC ---
void moveServo(int deg) {
  myServo.write(constrain(deg + SERVO_OFFSET, 0, 180));
}

int readDistance() {
  long sum = 0;
  for(int i=0; i<3; i++) {
    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false);
    int d = measure.RangeMilliMeter;
    if (measure.RangeStatus == 4 || d > 2000 || d < 40) d = 2000;
    sum += d; delay(10);
  }
  return sum / 3;
}

void fetchPrayerTimes() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://api.aladhan.com/v1/timingsByCity?city=Yogyakarta&country=Indonesia&method=4");
    if (http.GET() > 0) {
      JsonDocument doc;
      deserializeJson(doc, http.getString());
      JsonObject t = doc["data"]["timings"];
      String p[] = {"Fajr", "Dhuhr", "Asr", "Maghrib", "Isha"};
      for (int i = 0; i < 5; i++) {
        String s = t[p[i]].as<String>();
        schedule.hours[i] = s.substring(0, 2).toInt();
        schedule.mins[i] = s.substring(3, 5).toInt();
      }
    }
    http.end();
  }
}

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.println("SENTINEL-X v9.0");
  display.println("-------------------");
  display.display();
  
  ESP32PWM::allocateTimer(0);
  myServo.setPeriodHertz(50);
  myServo.attach(SERVO_PIN, 500, 2400); 
  moveServo(90);

  lox.begin(); bmp.begin();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); display.print("."); display.display(); }

  configTime(25200, 0, "id.pool.ntp.org");
  fetchPrayerTimes();
  timerInterval = millis();
}

void loop() {
  struct tm t;
  bool valid = getLocalTime(&t);
  float temp = bmp.readTemperature();

  display.clearDisplay();
  drawHeader(temp);

  if (currentState == SCANNING) {
    if (millis() > nextBlink) { isBlinking = !isBlinking; nextBlink = millis() + (isBlinking ? 150 : random(2000, 5000)); }
    drawLargeEyes(!isBlinking);

    display.setTextSize(1);
    display.setCursor(40, 42); if(valid) display.printf("%02d:%02d:%02d", t.tm_hour, t.tm_min, t.tm_sec);
    display.setCursor(0, 56); if(valid) display.printf("%02d-%02d", t.tm_mday, t.tm_mon + 1);
    display.setCursor(70, 56); display.printf("A:%d SCAN...", currentAngle);

    if (millis() - timerInterval > SCAN_STEP_TIME) {
      timerInterval = millis();
      if (isMovingState) { moveServo(currentAngle); isMovingState = false; }
      else {
        int d = readDistance();
        if (d < minDistance && d > 40) { minDistance = d; bestAngle = currentAngle; }
        currentAngle += 15; isMovingState = true;
        if (currentAngle > 180) currentState = LOCKING;
      }
    }
  }
  else if (currentState == LOCKING) {
    moveServo(bestAngle); currentAngle = 0; lockStartMillis = millis(); currentState = LOCKED_WAIT;
  }
  else if (currentState == LOCKED_WAIT) {
    int currTotal = (t.tm_hour * 3600) + (t.tm_min * 60) + t.tm_sec;
    nextPrayerDisplay = "";
    for (int i = 0; i < 5; i++) {
      int pray = (schedule.hours[i] * 3600) + (schedule.mins[i] * 60);
      if (pray > currTotal) {
        int diff = pray - currTotal;
        nextPrayerDisplay = schedule.names[i] + " -" + String(diff/60) + ":" + (diff%60 < 10 ? "0":"") + String(diff%60);
        break;
      }
    }
    display.setTextSize(3); display.setCursor(2, 22); display.printf("%02d:%02d", t.tm_hour, t.tm_min);
    display.setTextSize(2); display.setCursor(92, 30); display.printf(":%02d", t.tm_sec);
    display.setTextSize(1);
    display.setCursor(0, 56); if(valid) display.printf("%02d-%02d", t.tm_mday, t.tm_mon + 1);
    display.setCursor(85, 56); display.printf("LOK @%d", bestAngle);

    if (millis() - lockStartMillis > LOCK_DURATION) {
      minDistance = 2000; currentState = SCANNING; isMovingState = true; timerInterval = millis();
    }
  }
  display.display();
}