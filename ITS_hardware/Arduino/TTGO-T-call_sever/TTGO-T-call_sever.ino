#include <WiFi.h>
#include <HTTPClient.h>

// Replace with your network credentials
const char* ssid = "Impact School AP";
const char* password = "impact@123";
const char* serverIP = "192.168.1.56";  // Replace with the actual IP address of the ESP32-CAM

void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to Wi-Fi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Send a "Hello" request to the ESP32-CAM
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String("http://") + serverIP + "/";
    http.begin(url);
    int httpCode = http.GET();

    if(httpCode > 0) {
      String payload = http.getString();
      Serial.println("Response: " + payload);
    } else {
      Serial.println("Error on HTTP request");
    }
    http.end();
  }
}

void loop() {
  // Nothing to do here
}
