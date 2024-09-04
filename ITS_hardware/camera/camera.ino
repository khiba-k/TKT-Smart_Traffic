#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

void WiFiEvent(WiFiEvent_t event) {
  switch(event) {
    case WIFI_EVENT_STA_CONNECTED:
      Serial.println("Connected to WiFi network");
      break;
    case WIFI_EVENT_STA_DISCONNECTED:
      Serial.println("Disconnected from WiFi network");
      break;
    case IP_EVENT_STA_GOT_IP:
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      break;
    default:
      break;
  }
}

void setup() {
  Serial.begin(115200);

  // Set up WiFi event handler
  WiFi.onEvent(WiFiEvent);

  // Initialize the Wi-Fi module
  WiFi.begin(ssid, password);

  Serial.println("Connecting to WiFi...");
}

void loop() {
  // Your code here
}
