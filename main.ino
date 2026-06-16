#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// WiFi credentials
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Flask server endpoint
const char* serverUrl = "http://YOUR_SERVER_IP:5000/update";

// GPS module connected to D6 (RX) and D5 (TX) on NodeMCU
static const int RXPin = D6;
static const int TXPin = D5;
static const uint32_t GPSBaud = 9600;

// LED pin (built-in LED on NodeMCU)
const int LED_PIN = LED_BUILTIN;

TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
WiFiClient wifiClient;

unsigned long lastSendTime = 0;
const unsigned long sendInterval = 5000; // 5 seconds

void setup() {
  Serial.begin(115200);
  ss.begin(GPSBaud);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // LED off (active LOW on NodeMCU)

  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Feed GPS data
  while (ss.available() > 0) {
    gps.encode(ss.read());
  }

  unsigned long now = millis();

  if (now - lastSendTime >= sendInterval) {
    lastSendTime = now;

    if (gps.location.isValid()) {
      double lat   = gps.location.lat();
      double lng   = gps.location.lng();
      double speed = gps.speed.kmph();

      Serial.print("LAT: ");  Serial.println(lat, 6);
      Serial.print("LNG: ");  Serial.println(lng, 6);
      Serial.print("SPEED: "); Serial.print(speed); Serial.println(" km/h");

      // Blink LED to indicate GPS fix
      blinkLED(3);

      // Send data to Flask server
      sendGPSData(lat, lng, speed);
    } else {
      Serial.println("Waiting for GPS fix...");
      blinkLED(1);
    }
  }
}

void blinkLED(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, LOW);  // ON
    delay(150);
    digitalWrite(LED_PIN, HIGH); // OFF
    delay(150);
  }
}

void sendGPSData(double lat, double lng, double speed) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Skipping send.");
    return;
  }

  HTTPClient http;
  http.begin(wifiClient, serverUrl);
  http.addHeader("Content-Type", "application/json");

  String payload = "{";
  payload += "\"latitude\":"  + String(lat, 6) + ",";
  payload += "\"longitude\":" + String(lng, 6) + ",";
  payload += "\"speed\":"     + String(speed, 2);
  payload += "}";

  int httpCode = http.POST(payload);

  if (httpCode > 0) {
    Serial.print("HTTP POST response: ");
    Serial.println(httpCode);
    if (httpCode == HTTP_CODE_OK) {
      Serial.println("Data sent successfully.");
    }
  } else {
    Serial.print("HTTP POST failed, error: ");
    Serial.println(http.errorToString(httpCode).c_str());
  }

  http.end();
}
