#define LED_PIN 4

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);

  // Initialize the LED pin as an output
  pinMode(LED_PIN, OUTPUT);

  // Turn the LED off initially
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  // Turn the LED on
  Serial.println("LED on");
  digitalWrite(LED_PIN, LOW);  // LOW typically turns the LED on for the ESP32-CAM
  delay(2000);                 // Keep the LED on for 2 seconds

  // Turn the LED off
  Serial.println("LED off");
  digitalWrite(LED_PIN, HIGH); // HIGH typically turns the LED off for the ESP32-CAM
  delay(2000);                 // Keep the LED off for 2 seconds
}
