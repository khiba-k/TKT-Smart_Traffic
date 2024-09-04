#define FLASH_LED_PIN 4

void setup() {
  pinMode(FLASH_LED_PIN, OUTPUT);
  digitalWrite(FLASH_LED_PIN, LOW); // Turn off the flash
}

void loop() {
  digitalWrite(FLASH_LED_PIN, HIGH); // Turn on the flash
  delay(1000); // Keep it on for 1 second
  digitalWrite(FLASH_LED_PIN, LOW); // Turn off the flash
  delay(1000); // Keep it off for 1 second
}
