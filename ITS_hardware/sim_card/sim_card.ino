#define TINY_GSM_MODEM_SIM800

#include <Arduino.h>
#include <TinyGsmClient.h>
#include <WiFi.h>
#include <ArduinoHttpClient.h>

// Select your modem:


// TTGO T-Call pins
#define MODEM_RST            5
#define MODEM_PWRKEY         4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26
#define I2C_SDA              21
#define I2C_SCL              22
#define LED_GPIO             13
#define LED_ON               HIGH
#define LED_OFF              LOW

#define SerialMon            Serial
#define SerialAT             Serial1

// WiFi credentials
const char wifiSSID[] = "Impact School AP";
const char wifiPass[] = "impact@123";

// Server details
const char server[] = "192.168.1.53";
const int port = 5000;

#define DUMP_AT_COMMANDS
#define TINY_GSM_DEBUG SerialMon

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

TinyGsmClient client(modem);
HttpClient http(client, server, port);

void setupModem() {
  pinMode(MODEM_PWRKEY, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);

  digitalWrite(MODEM_PWRKEY, LOW);
  digitalWrite(MODEM_POWER_ON, HIGH);
  digitalWrite(MODEM_RST, HIGH);

  // To turn on the modem, PWRKEY should be low for at least 1 second and then high
  digitalWrite(MODEM_PWRKEY, HIGH);
  delay(100);
  digitalWrite(MODEM_PWRKEY, LOW);
  delay(1000);
  digitalWrite(MODEM_PWRKEY, HIGH);

  pinMode(LED_GPIO, OUTPUT);
  digitalWrite(LED_GPIO, LED_OFF);
}

void setup() {
  // Set console baud rate
  SerialMon.begin(115200);
  delay(10);

  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

  setupModem();
  delay(6000);

  SerialMon.println("Initializing modem...");
  modem.restart();

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

  SerialMon.print(F("Connecting to WiFi..."));
  WiFi.begin(wifiSSID, wifiPass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    SerialMon.print(".");
  }
  SerialMon.println(" connected");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    SerialMon.println("WiFi connected");

    SerialMon.println("Performing HTTP GET request...");

    http.get("/");

    int statusCode = http.responseStatusCode();
    String response = http.responseBody();

    SerialMon.print("Status code: ");
    SerialMon.println(statusCode);
    SerialMon.print("Response: ");
    SerialMon.println(response);

    // Disconnect WiFi to save power (optional)
    WiFi.disconnect();
    SerialMon.println(F("WiFi disconnected"));

    // Do nothing forevermore
    while (true) {
      delay(1000);
    }
  } else {
    SerialMon.println("WiFi not connected");
    delay(10000);
  }
}
