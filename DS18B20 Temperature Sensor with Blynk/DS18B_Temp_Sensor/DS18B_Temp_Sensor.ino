#define BLYNK_TEMPLATE_ID "TMPL6CuEc0mLm"
#define BLYNK_TEMPLATE_NAME "DS18B20 Temp Sensor"
#define BLYNK_AUTH_TOKEN "f9cmOV8ZeyLVHUTyub15K-cN3Xs79bYD"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// DS18B20 connected to GPIO 4
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// WiFi credentials
char ssid[] = "RoboTech";
char pass[] = "aslam12345";

BlynkTimer timer;

void sendTemperature() {
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  float tempF = sensors.toFahrenheit(tempC);

  Blynk.virtualWrite(V0, tempC); // Celsius
  Blynk.virtualWrite(V1, tempF); // Fahrenheit

  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" | Temp F: ");
  Serial.println(tempF);
}

void setup() {
  Serial.begin(115200);
  sensors.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(500L, sendTemperature); // Every 2 seconds
}

void loop() {
  Blynk.run();
  timer.run();
}
