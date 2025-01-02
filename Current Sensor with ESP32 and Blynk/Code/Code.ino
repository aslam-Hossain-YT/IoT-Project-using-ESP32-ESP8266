// Blynk credentials
#define BLYNK_TEMPLATE_ID "TMPL68d_K4XwI"
#define BLYNK_TEMPLATE_NAME "s"
#define BLYNK_AUTH_TOKEN "C1JGFBBoE1Jaa-FPk0SQOcUMPwzzmhfZ"

// Code snippet for current measurement using ACS712 and sending data to Blynk
#include "ACS712.h"
#include <BlynkSimpleEsp32.h>


char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Tplink";
char pass[] = "0187754ss@@";

ACS712 ACS(34, 3.3, 4095, 125);  // Parameters: analog pin, Vref, ADC resolution, sensor sensitivity
int calibration_factor = 120;

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();

  float current = readCurrent();
  Blynk.virtualWrite(V0, current);  // Send current value to Virtual Pin V1
}

float readCurrent() {
  float average = 0;
  for (int i = 0; i < 100; i++) {
    average += ACS.mA_AC();
  }
  float mA = (abs(average / 100.0) - calibration_factor);
  if (mA <= 5) mA = 0;

  // Print values for debugging
  Serial.print("Current: ");
  Serial.print(mA);
  Serial.println(" mA");

  return mA;
}