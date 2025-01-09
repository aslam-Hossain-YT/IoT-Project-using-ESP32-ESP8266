#define BLYNK_TEMPLATE_ID "TMPL6SvZE2Yt6"
#define BLYNK_TEMPLATE_NAME "Water Level Monitoring"
#define BLYNK_AUTH_TOKEN "3bgnwVYrQoCACVzI5e544720G8O-03Pu"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// WiFi credentials
char ssid[] = "Tplink";      // Replace with your WiFi SSID
char pass[] = "12345678";  // Replace with your WiFi password

// Ultrasonic sensor pins
const int trigPin = 33; // Trigger pin of the ultrasonic sensor
const int echoPin = 25; // Echo pin of the ultrasonic sensor

// Tank depth in centimeters
const float tankDepth = 12;

BlynkTimer timer;

// Function to calculate water level percentage and send it to Blynk
void sendWaterLevel() {
  // Trigger the ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the duration of the echo signal
  long duration = pulseIn(echoPin, HIGH);

  // Calculate the distance in centimeters
  float distance = duration * 0.034 / 2;

  // Calculate the water level percentage
  float percentage = ((tankDepth - distance) / tankDepth) * 100;

  // Clamp the percentage value between 0 and 100
  if (percentage < 0) percentage = 0;
  if (percentage > 100) percentage = 100;

  // Send the percentage value to Blynk
  Blynk.virtualWrite(V0, percentage);

  // Print the percentage value to the Serial Monitor
  Serial.print("Water Level: ");
  Serial.print(percentage);
  Serial.println("%");
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Connect to WiFi and Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Set up a timer to call the sendWaterLevel function every second
  timer.setInterval(1000L, sendWaterLevel);
}

void loop() {
  Blynk.run();
  timer.run();
}
