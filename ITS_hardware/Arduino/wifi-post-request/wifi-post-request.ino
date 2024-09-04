#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

// WiFi credentials
const char wifiSSID[] = "SLICE";
const char wifiPass[] = "password";

const int trigPin = 23; // GPIO 23 on TTGO T-CALL
const int echoPin = 22; // GPIO 22 on TTGO T-CALL

unsigned long previousMillis = 0;
const long interval = 500; // Interval in milliseconds (5 seconds)

float totalSpeed = 0.0; // Variable to accumulate total speed for new distance calculation
int speedIndex = 0; // Index to keep track of speed measurements
float averageSpeed;

float oldDistance = 0.0; // Variable to store the previous distance
float distance = 0.0; // Current distance

// Server details
const char server[] = "https://h27x0zfg-5000.inc1.devtunnels.ms/traffic_speed3/";

// Hardware Serial on TTGO T-Call
#define MODEM_RST             5
#define MODEM_PWRKEY          4
#define MODEM_POWER_ON        23
#define MODEM_TX              27
#define MODEM_RX              26
#define MODEM_BAUD            115200

#define SerialMon             Serial
#define SerialAT              Serial1

void setupModem() {
    pinMode(MODEM_PWRKEY, OUTPUT);
    pinMode(MODEM_POWER_ON, OUTPUT);
    pinMode(MODEM_RST, OUTPUT);

    digitalWrite(MODEM_PWRKEY, HIGH);
    digitalWrite(MODEM_POWER_ON, HIGH);
    digitalWrite(MODEM_RST, HIGH);

    delay(100);
    digitalWrite(MODEM_PWRKEY, LOW);
    delay(1000);
    digitalWrite(MODEM_PWRKEY, HIGH);
}

void setup() {
    // Set console baud rate
    SerialMon.begin(115200);
    delay(10);

    Serial.begin(115200);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    SerialMon.println("Initializing modem...");
    setupModem();
    delay(6000);

    // Initialize the WiFi
    SerialMon.print(F("Connecting to WiFi..."));
    WiFi.begin(wifiSSID, wifiPass);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        SerialMon.print(".");
    }

    SerialMon.println("Connected to WiFi");(distance < oldDistance);
    SerialMon.print("IP Address: ");
    SerialMon.println(WiFi.localIP());
}

void speed_loop() {
    long duration;

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // Check if the distance is decreasing (car is moving towards the sensor)
     if (distance > oldDistance && speedIndex > 0) {
        // Car has passed by, calculate the average speed and reset values
        averageSpeed = ((abs(totalSpeed / speedIndex)) / 100000) * 3600;

        Serial.print("Average Speed: ");
        Serial.print(averageSpeed);
        Serial.println(" km/h");

        // Perform HTTP POST request
        if (WiFi.status() == WL_CONNECTED) {
            SerialMon.println("Performing HTTP POST request...");

            HTTPClient http;
            http.begin(server);

            // Define the JSON data to be sent
            String postData = "{\"traffic_speed3\":" + String(averageSpeed) + "}";

            // Set content type to application/json
            http.addHeader("Content-Type", "application/json");

            // Send the request-+
            int httpResponseCode = http.POST(postData);

            // Check the response code
            SerialMon.print("HTTP Response code: ");
            SerialMon.println(httpResponseCode);

            // Free resources
            http.end();
        } else {
            SerialMon.println("WiFi not connected");
        }

        // Reset for the next car
        totalSpeed = 0.0;
        speedIndex = 0;
        oldDistance = distance;
    }

    else {
        float speed = abs((oldDistance - distance)) / (interval / 1000.0); // Convert interval to seconds for speed calculation
        totalSpeed += speed;
        speedIndex++;
        oldDistance = distance;

        Serial.print("Speed: ");
        Serial.print(speed);
        Serial.println(" cm/s");
        Serial.print("Total Speed: ");
        Serial.print(totalSpeed);
        Serial.println(" cm/s");
    }
}

void loop() {
    // Perform speed calculation in speed_loop
    speed_loop();

    // Wait for the interval before repeating the measurement
    delay(interval);
}
