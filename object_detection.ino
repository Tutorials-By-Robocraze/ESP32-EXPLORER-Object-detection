#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "esp_camera.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Replace with your network credentials
const char* ssid = "your-SSID";
const char* password = "your-PASSWORD";

// Your server URL
const char* serverUrl = "http://your-server-url.com/detect"; // Replace with your object detection server
URL

void setup() {
Serial.begin(115200);

// Initialize OLED display
if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
Serial.println(F("SSD1306 allocation failed"));
for(;;);
}
display.display();
delay(2000);
display.clearDisplay();

// Connect to Wi-Fi
WiFi.begin(ssid, password);

display.setTextSize(1);
display.setTextColor(SSD1306_WHITE);
display.setCursor(0, 0);
display.print("Connecting to WiFi...");
display.display();

while (WiFi.status() != WL_CONNECTED) {
delay(1000);
Serial.println("Connecting to WiFi...");
display.setCursor(0, 10);
display.print(".");
display.display();
}
Serial.println("Connected to WiFi");
display.clearDisplay();
display.setCursor(0, 0);
display.print("WiFi connected!");
display.display();

// Initialize the camera
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
config.pin_xclk_freq_hz = 20000000;
config.pixel_format = PIXFORMAT_JPEG;

if (esp_camera_init(&config) != ESP_OK) {
Serial.println("Camera init failed");
display.clearDisplay();
display.setCursor(0, 0);
display.print("Camera init failed");
display.display();
return;
}
}

void loop() {
// Capture an image
camera_fb_t * fb = esp_camera_fb_get();
if (!fb) {
Serial.println("Camera capture failed");
display.clearDisplay();
display.setCursor(0, 0);
display.print("Capture failed");
display.display();
return;
}

// Send image to server for object detection
if (WiFi.status() == WL_CONNECTED) {
HTTPClient http;
http.begin(serverUrl);

http.addHeader("Content-Type", "image/jpeg");

int httpResponseCode = http.POST(fb->buf, fb->len);
if (httpResponseCode > 0) {
String response = http.getString();
Serial.println(httpResponseCode);
Serial.println(response);

// Display result on OLED
display.clearDisplay();
display.setCursor(0, 0);
display.print("Response:");
display.setCursor(0, 10);
display.print(response);
display.display();
} else {
Serial.print("Error code: ");
Serial.println(httpResponseCode);
display.clearDisplay();
display.setCursor(0, 0);
display.print("HTTP Error: ");
display.print(httpResponseCode);
display.display();
}
http.end();
}

// Return the frame buffer to be reused
esp_camera_fb_return(fb);

delay(10000); // Wait 10 seconds before capturing the next image
}