/*
  ============================================================
  Electrical Line Fatigue Monitoring System
  Arduino Uno — With NodeMCU Communication
  ============================================================
*/

#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

// ── LCD
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

// ── SoftwareSerial for NodeMCU
SoftwareSerial nodemcu(12, 13); // RX=12, TX=13

// ── Analog Pins
const int TEMP_PIN    = A0;
const int CURRENT_PIN = A2;

// ── Digital Output Pins
const int LED_GREEN  = 8;
const int LED_YELLOW = 9;
const int LED_RED    = 10;
const int BUZZER     = 11;

// ── Thresholds
const float TEMP_FATIGUE = 63.0;
const float TEMP_DANGER  = 65.0;
const float CURR_FATIGUE = 2.5;
const float CURR_DANGER  = 4.0;

void setup() {
  pinMode(LED_GREEN,  OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED,    OUTPUT);
  pinMode(BUZZER,     OUTPUT);

  // Startup LED sequence
  digitalWrite(LED_GREEN,  HIGH); delay(400);
  digitalWrite(LED_GREEN,  LOW);
  digitalWrite(LED_YELLOW, HIGH); delay(400);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED,    HIGH); delay(400);
  digitalWrite(LED_RED,    LOW);

  // Startup beep
  digitalWrite(BUZZER, HIGH); delay(300);
  digitalWrite(BUZZER, LOW);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("  Line Fatigue  ");
  lcd.setCursor(0, 1);
  lcd.print(" Monitor v1.0   ");
  delay(2000);
  lcd.clear();

  Serial.begin(9600);
  nodemcu.begin(9600);
  Serial.println("System Ready.");
}

void loop() {

  // 1. Read Temperature
  int rawTemp       = analogRead(TEMP_PIN);
  float voltage_T   = (rawTemp / 1023.0) * 5.0;
  float temperature = voltage_T * 100.0;

  // 2. Read Current
  int rawCurr   = analogRead(CURRENT_PIN);
  float current = (rawCurr / 1023.0) * 13.5;

  // 3. Determine Status
  bool isDanger  = (temperature >= TEMP_DANGER  || current >= CURR_DANGER);
  bool isFatigue = (temperature >= TEMP_FATIGUE || current >= CURR_FATIGUE);

  String status;
  int statusCode;
  if (isDanger) {
    status = "DANGER ";
    statusCode = 2;
  } else if (isFatigue) {
    status = "FATIGUE";
    statusCode = 1;
  } else {
    status = "SAFE   ";
    statusCode = 0;
  }

  // 4. Drive LEDs + Buzzer
  if (isDanger) {
    digitalWrite(LED_GREEN,  LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED,    HIGH);
    digitalWrite(BUZZER,     HIGH);
    delay(100);
    digitalWrite(LED_RED,    LOW);
    digitalWrite(BUZZER,     LOW);
    delay(100);
  } else if (isFatigue) {
    digitalWrite(LED_GREEN,  LOW);
    digitalWrite(LED_YELLOW, HIGH);
    digitalWrite(LED_RED,    LOW);
    digitalWrite(BUZZER,     HIGH);
    delay(300);
    digitalWrite(BUZZER,     LOW);
    delay(300);
  } else {
    digitalWrite(LED_GREEN,  HIGH);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED,    LOW);
    digitalWrite(BUZZER,     LOW);
  }

  // 5. LCD Display
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print("C I:");
  lcd.print(current, 2);
  lcd.print("A  ");

  lcd.setCursor(0, 1);
  lcd.print("Status: ");
  lcd.print(status);

  // 6. Send to NodeMCU
  nodemcu.print(temperature, 1);
  nodemcu.print(",");
  nodemcu.print(current, 2);
  nodemcu.print(",");
  nodemcu.println(statusCode);

  // 7. Serial Dashboard
  Serial.println("============================");
  Serial.print("TEMP    : "); Serial.print(temperature, 1); Serial.println(" C");
  Serial.print("CURRENT : "); Serial.print(current, 2); Serial.println(" A");
  Serial.print("STATUS  : "); Serial.println(status);
  Serial.println("============================");

  delay(300);
}