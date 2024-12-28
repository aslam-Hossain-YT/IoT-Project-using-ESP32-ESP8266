#define BLYNK_TEMPLATE_ID "TMPL6IANUr1h7"
#define BLYNK_TEMPLATE_NAME "IoT Based Smart Irrigation System"
#define BLYNK_AUTH_TOKEN "Y9FX6lFgkaypR1M6mB_tGs71zFwfjN4C"

// Automatic Plant Watering System using ESP32 and Blynk App
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

#define SOIL_MOISTURE_PIN 34 // Analog pin connected to soil moisture sensor

// Blynk credentials
char auth[] = BLYNK_AUTH_TOKEN; // Replace with your Blynk authentication token
char ssid[] = "Tplink";         // Replace with your WiFi SSID
char pass[] = "0187754ss@@";    // Replace with your WiFi password

// Blynk Virtual Pin
#define VPIN_GAUGE V2 // Virtual pin for soil moisture gauge

// Function to read soil moisture level
int getSoilMoisture()
{
  int rawValue = analogRead(SOIL_MOISTURE_PIN);
  return map(rawValue, 4095, 0, 0, 100); // Adjust based on your sensor's calibration
}

void setup()
{
  Serial.begin(115200);

  // Setup pin mode
  pinMode(SOIL_MOISTURE_PIN, INPUT);

  // Connect to WiFi and Blynk
  Blynk.begin(auth, ssid, pass);
}

void loop()
{
  Blynk.run();

  // Read soil moisture level
  int moistureLevel = getSoilMoisture();

  // Send soil moisture data to Blynk
  Blynk.virtualWrite(VPIN_GAUGE, moistureLevel);

  // Log data to Serial Monitor
  Serial.print("Soil Moisture: ");
  Serial.print(moistureLevel);
  Serial.println("%");

  delay(1000); // Adjust delay as needed
}
