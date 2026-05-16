/*************************************************************
  NodeMCU ESP8266 + Blynk
  Electrical Line Fatigue Monitoring System
*************************************************************/

#define BLYNK_TEMPLATE_ID "TMPLxxxxxx"
#define BLYNK_TEMPLATE_NAME "Electrical Line Fatigue"
#define BLYNK_AUTH_TOKEN "YourAuthToken"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// WiFi Credentials
char ssid[] = "YourWiFiName";
char pass[] = "YourWiFiPassword";

// Variables
float temperature = 0;
float current = 0;
String statusValue = "SAFE";

BlynkTimer timer;

// Read serial data from Arduino
void readArduinoData()
{
  if (Serial.available())
  {
    String data = Serial.readStringUntil('\n');

    // Example Format:
    // Temp: 35.2 C | Current: 1.50 A | Status: SAFE

    int tIndex = data.indexOf("Temp:");
    int cIndex = data.indexOf("Current:");
    int sIndex = data.indexOf("Status:");

    if (tIndex >= 0 && cIndex >= 0 && sIndex >= 0)
    {
      // Extract Temperature
      String tempStr = data.substring(tIndex + 5, data.indexOf("C"));
      temperature = tempStr.toFloat();

      // Extract Current
      String currStr = data.substring(cIndex + 8, data.indexOf("A"));
      current = currStr.toFloat();

      // Extract Status
      statusValue = data.substring(sIndex + 7);
      statusValue.trim();

      // Send to Blynk
      Blynk.virtualWrite(V0, temperature);
      Blynk.virtualWrite(V1, current);
      Blynk.virtualWrite(V2, statusValue);

      // Notifications
      if (statusValue == "DANGER")
      {
        Blynk.logEvent("danger_alert",
                       "Electrical Line in DANGER condition!");
      }

      else if (statusValue == "FATIGUE")
      {
        Blynk.logEvent("fatigue_alert",
                       "Electrical Line in FATIGUE condition!");
      }

      // Serial Debug
      Serial.println(data);
    }
  }
}

void setup()
{
  // Serial Communication
  Serial.begin(9600);

  // Connect to WiFi & Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Timer
  timer.setInterval(1000L, readArduinoData);
}

void loop()
{
  Blynk.run();
  timer.run();
}