#include <Arduino.h>

const int trigPin = 23; // GPIO 23 on TTGO T-CALL
const int echoPin = 22; // GPIO 22 on TTGO T-CALL

unsigned long previousMillis = 0;
const long interval = 50; // Interval in milliseconds (5 seconds)

float totalSpeed = 0.0; // Variable to accumulate total speed for new distance calculation
float previousSpeed = 0.0; // Variable to store previous speed for comparison
int speedIndex = 0; // Index to keep track of speed measurements

float oldDistance = 0.0; // Variable to store the previous new_distance

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  long duration, distance;

  unsigned long currentMillis = millis();

  // Check if 5 seconds have elapsed
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

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

    // Check if new distance is greater than old distance and reset calculation
    if (distance > oldDistance) {
      totalSpeed = 0.0; // Reset total speed for new distance calculation
      speedIndex = 0; // Reset speed index for new distance calculation
      oldDistance = distance; // Update oldDistance with the new distance
    }

    // Calculate speed
    float speed = distance / 5.0; // Assuming interval is 5 seconds

    // Check if speed is constant compared to previous speed
    if (abs(speed - previousSpeed) > 0.1) { // Adjust threshold for acceptable variation
      // Speed changed, update total speed, index, and previous speed
      totalSpeed += speed;
      speedIndex++;
      previousSpeed = speed;
    }

    Serial.print("Speed: ");
    Serial.print(speed);
    Serial.println(" cm/s");

    // Calculate average speed only if there are speed measurements
    if (speedIndex > 0) {
      float averageSpeed = totalSpeed / speedIndex; // Calculate average speed
      Serial.print("Average Speed: ");
      Serial.print(averageSpeed);
      Serial.println(" cm/s");
    }
  }

  delay(100); // Adjust delay to control loop timing
}
