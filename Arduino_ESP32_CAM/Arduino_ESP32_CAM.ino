#include "esp_camera.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Camera-related constants
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

#define FLASH_PIN 4  // Flashlight pin

const char *ssid = "BOAT";
const char *password = "12345678";

AsyncWebServer server(80);
AsyncWebSocket wsCamera("/Camera");

uint32_t cameraClientId = 0;

// HTML for the webpage
const char *htmlPage PROGMEM = R"HTML(
<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
      body { font-family: Arial; text-align: center; background-color: #f0f0f0; margin: 0; padding: 0; }
      h2 { color: teal; }
      .slidecontainer { width: 80%; margin: 20px auto; }
      .slider { width: 100%; }
      img { max-width: 100%; height: auto; }
    </style>
  </head>
  <body>
    <h2>Fish Feeding Boat</h2>
    <img id="cameraImage" src="" alt="Video Stream">
    <div class="slidecontainer">
      <p><b>Flashlight Brightness:</b></p>
      <input type="range" min="0" max="255" value="0" class="slider" id="flashlightSlider" 
             oninput="adjustFlashlight(this.value)">
    </div>
    <script>
      const cameraSocketUrl = `ws://${window.location.hostname}/Camera`;
      let cameraSocket;

      function initCameraSocket() {
        cameraSocket = new WebSocket(cameraSocketUrl);
        cameraSocket.binaryType = "blob";
        cameraSocket.onmessage = function(event) {
          document.getElementById("cameraImage").src = URL.createObjectURL(event.data);
        };
        cameraSocket.onclose = function() {
          setTimeout(initCameraSocket, 2000);
        };
      }

      function adjustFlashlight(brightness) {
        fetch(`/controlFlashlight?value=${brightness}`);
      }

      window.onload = initCameraSocket;
    </script>
  </body>
</html>
)HTML";

// Function to control the flashlight
void controlFlashlight(int brightness) {
  ledcWrite(0, brightness);
}

// Initialize the camera
bool initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // Frame size and quality
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 10;
  config.fb_count = 2;

  // Initialize the camera
  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init failed!");
    return false;
  }
  return true;
}

void setup() {
  Serial.begin(115200);

  // Setup LEDC for flashlight control
  ledcSetup(0, 5000, 8);
  ledcAttachPin(FLASH_PIN, 0);

  // Initialize camera
  if (!initCamera()) {
    Serial.println("Failed to initialize the camera!");
    while (1);
  }

  // Start WiFi in AP mode
  WiFi.softAP(ssid, password);
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());

  // WebSocket event handler for camera stream
  wsCamera.onEvent([](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
      cameraClientId = client->id();
    } else if (type == WS_EVT_DISCONNECT) {
      cameraClientId = 0;
    }
  });
  server.addHandler(&wsCamera);

  // Route for the HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", htmlPage);
  });

  // Route for controlling the flashlight
  server.on("/controlFlashlight", HTTP_GET, [](AsyncWebServerRequest *request) {
    int brightness = request->getParam("value")->value().toInt();
    controlFlashlight(brightness);
    request->send(200, "text/plain", "Flashlight brightness set");
  });

  server.begin();
}

void loop() {
  wsCamera.cleanupClients();

  // Capture and send frames if a client is connected
  if (cameraClientId) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (fb) {
      wsCamera.binary(cameraClientId, fb->buf, fb->len);
      esp_camera_fb_return(fb);
    }
  }
}

