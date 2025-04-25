#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

// DHT Settings
#define DHTPIN 4         // GPIO4 = D2 on NodeMCU
#define DHTTYPE DHT11    
DHT dht(DHTPIN, DHTTYPE);

// WiFi Access Point Credentials
const char* ssid = "ESP8266_Weather_AP";
const char* password = "12345678";

// Web Server
ESP8266WebServer server(80);

// HTML generator with comment
String getHTML(float tempC, float tempF, float hum, String comment) {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>ESP8266 Weather Station</title>
      <style>
        body {
          font-family: 'Segoe UI', sans-serif;
          background: linear-gradient(to top, #c9d6ff, #e2e2e2);
          margin: 0;
          padding: 0;
          display: flex;
          flex-direction: column;
          align-items: center;
          justify-content: center;
          height: 100vh;
        }
        .weather-box {
          background: white;
          padding: 30px;
          border-radius: 15px;
          box-shadow: 0 8px 20px rgba(0, 0, 0, 0.1);
          text-align: center;
        }
        h1 {
          margin-bottom: 20px;
          color: #333;
        }
        .data {
          font-size: 24px;
          margin: 15px 0;
          color: #555;
        }
        .comment {
          margin-top: 20px;
          font-size: 20px;
          color: #0077aa;
        }
      </style>
    </head>
    <body>
      <div class="weather-box">
        <h1>🌦️ IoT Weather Station</h1>
        <div class="data">🌡️ Temperature: )rawliteral" + String(tempC, 1) + " &deg;C / " + String(tempF, 1) + R"rawliteral( &deg;F</div>
        <div class="data">💧 Humidity: )rawliteral" + String(hum, 1) + R"rawliteral( %</div>
        <div class="comment">)rawliteral" + comment + R"rawliteral(</div>
      </div>
    </body>
    </html>
  )rawliteral";
  return html;
}

void handleRoot() {
  float tempC = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(tempC) || isnan(hum)) {
    server.send(500, "text/plain", "DHT sensor read failed");
    return;
  }

  float tempF = tempC * 9.0 / 5.0 + 32.0;

  // Determine weather comment
  String comment;
  if (tempC >= 30) {
    comment = "It's a hot day! Stay hydrated. ☀️";
  } else if (tempC >= 20) {
    comment = "Nice and cool today. 😌";
  } else {
    comment = "Bit chilly, dress warm! 🧥";
  }

  server.send(200, "text/html", getHTML(tempC, tempF, hum, comment));
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.softAP(ssid, password);
  delay(100);
  Serial.println("AP Mode Started. IP address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.begin();
  Serial.println("Web server started.");
}

void loop() {
  server.handleClient();
}
