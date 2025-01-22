#define BLYNK_TEMPLATE_ID "TMPL6xoe6xKfN"
#define BLYNK_TEMPLATE_NAME "Energy Meter"
#define BLYNK_AUTH_TOKEN "bQOVv0MiDhQgf4W1jl6ISDs184UdsbYZ"

#include <WiFi.h>
#include "time.h"
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "ACS712.h"
#include <ZMPT101B.h>
#include <EEPROM.h>

// Your WiFi credentials.
char ssid[] = "Tplink";
char pass[] = "0187754ss@@";

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V0);
  Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V3);
}

ACS712 ACS(34, 3.3, 4095, 125);
ZMPT101B voltageSensor(35, 50.0);

LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD I2C address and size

float unit;
int volt, current, power;

#define EEPROM_SIZE 512
#define UNIT_ADDRESS 0  // Address in EEPROM to store unit value

void setup() {
  Serial.begin(115200);

  EEPROM.begin(EEPROM_SIZE);

  // Read the previous unit value from EEPROM
  unit = EEPROM.readFloat(UNIT_ADDRESS);
  if (isnan(unit)) {
    unit = 0.0;  // Initialize to 0 if EEPROM value is invalid
  }

  Serial.print("Previous Unit Value: ");
  Serial.println(unit);

  Serial.print("ACS712_LIB_VERSION: ");
  Serial.println(ACS712_LIB_VERSION);

  ACS.autoMidPoint();
  Serial.print("MidPoint: ");
  Serial.println(ACS.getMidPoint());
  Serial.print("Noise mV: ");
  Serial.println(ACS.getNoisemV());

  voltageSensor.setSensitivity(500.0f);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Energy Meter");
  delay(1000);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  Blynk.run();
  int noise = ACS.getNoisemV();
  float average = 0;
  for (int i = 0; i < 100; i++) {
    average += ACS.mA_AC();
  }
  float mA = (average / 100.0) - noise;

  (mA > 5) ? current = mA : current = 0;

  float voltage = voltageSensor.getRmsVoltage();
  (voltage > 50) ? volt = voltage : volt = 0;

  float watt = voltage * (mA / 1000.0);
  power = watt;
  float kWh = watt / 3600;
  unit += kWh;

  // Update LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("V:");
  lcd.print(volt);
  lcd.print("V C:");
  lcd.print(current);
  lcd.print("mA");

  lcd.setCursor(0, 1);
  lcd.print("P:");
  lcd.print(power);
  lcd.print("W U:");
  lcd.print(unit, 2);
  lcd.print("kWh");

  // Print to Serial Monitor
  Serial.print("Voltage: ");
  Serial.print(volt);
  Serial.println(" V");

  Serial.print("Current: ");
  Serial.print(current);
  Serial.println(" mA");

  Serial.print("Power: ");
  Serial.print(power);
  Serial.println(" W");

  Serial.print("Energy: ");
  Serial.print(unit, 4);
  Serial.println(" kWh");

  delay(500);

  // Save the current unit value to EEPROM
  EEPROM.writeFloat(UNIT_ADDRESS, unit);
  EEPROM.commit();

  Blynk.virtualWrite(V0, volt);
  Blynk.virtualWrite(V1, current);
  Blynk.virtualWrite(V2, power);
  Blynk.virtualWrite(V3, unit);
}
