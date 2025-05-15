#define BLYNK_TEMPLATE_ID "TMPL6vSyFiqSG"
#define BLYNK_TEMPLATE_NAME "MAX30100 Pulse Oximeter"
#define BLYNK_AUTH_TOKEN "8pr62XDYST9Gq574GY-uSc3VKrukKPtb"

#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "Tplink";
char pass[] = "0187754ss@@";

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define REPORTING_PERIOD_MS 1000

PulseOximeter pox;
uint32_t tsLastReport = 0;

void onBeatDetected() {
  Serial.println("Beat!");
}

void setup() {
  Serial.begin(115200);

  // Connect to WiFi and Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Initializing...");
  display.display();

  // Initialize MAX30100
  if (!pox.begin()) {
    Serial.println("MAX30100 FAILED");
    display.setCursor(0, 10);
    display.println("MAX30100 FAILED");
    display.display();
    while (true);
  } else {
    Serial.println("MAX30100 SUCCESS");
    display.setCursor(0, 10);
    display.println("MAX30100 SUCCESS");
    display.display();
  }

  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
  pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop() {
  Blynk.run();
  pox.update();

  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    float hr = pox.getHeartRate();
    float spo2 = pox.getSpO2();

    Serial.print("Heart rate: ");
    Serial.print(hr);
    Serial.print(" bpm / SpO2: ");
    Serial.print(spo2);
    Serial.println(" %");

    // Send to Blynk Cloud
    Blynk.virtualWrite(V0, hr);
    Blynk.virtualWrite(V1, spo2);

    // OLED Display
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("Pulse Oximeter");
    display.setTextSize(2);
    display.setCursor(0, 20);
    display.print("HR: ");
    display.print((int)hr);
    display.println(" bpm");
    display.setCursor(0, 45);
    display.print("SpO2: ");
    display.print((int)spo2);
    display.println(" %");
    display.display();

    tsLastReport = millis();
  }
}

