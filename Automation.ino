#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <BH1750.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>

// Motor and relay pin definitions for window control
#define relayPin 26       // Relay for window motor (Pin updated to previous guidance)
#define motorEnable 5     // Enable pin for window motor
#define motorIn1 14       // Motor input for window forward (Pin updated)
#define motorIn2 12       // Motor input for window reverse (Pin updated)

// Motor and relay pin definitions for blind control
#define blindRelayPin 27  // Relay for blind motor (Pin updated)
#define blindEnable 4     // Enable pin for blind motor
#define blindIn1 13       // Motor input for blind roll-up (Pin updated)
#define blindIn2 15       // Motor input for blind roll-down (Pin updated)

// Sensor instances
Adafruit_BME280 bme; // I2C
BH1750 lightMeter;

// NTP client setup
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800; // Adjust for your timezone (example is +5:30 GMT for India)
const int daylightOffset_sec = 0;

// Threshold values for conditions
const float tempHot = 20.0;
const float tempWarm = 25.0;
const float tempCool = 10.0;
const float humidityRainy = 80.0;
const float pressureRainy = 1000.0;
const float pressureWarmMin = 1008.0;
const float pressureWarmMax = 1018.0;
const float lightLow = 5000.0; // Light intensity threshold for rolling up blind

// WiFi credentials
const char* ssid = "hazelheo";       
const char* password = "Symphony"; 

// Firebase project credentials
const String FIREBASE_HOST = "https://project-45d44-default-rtdb.asia-southeast1.firebasedatabase.app/sensor";
const String FIREBASE_AUTH = "AIzaSyDP2IKJUiQwJGEZySs3F3jH88_dm8jiBoo";

// Timing variables
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
const long interval = 5000; // 5 seconds interval for sending data to Firebase

// State management for window and blind
bool windowOpen = false;
bool blindOpen = false;

void setup() {
  Serial.begin(115200);

  // Initialize motor control pins as outputs
  pinMode(relayPin, OUTPUT);
  pinMode(motorEnable, OUTPUT);
  pinMode(motorIn1, OUTPUT);
  pinMode(motorIn2, OUTPUT);
  
  // Initialize blind control pins as outputs
  pinMode(blindRelayPin, OUTPUT);
  pinMode(blindEnable, OUTPUT);
  pinMode(blindIn1, OUTPUT);
  pinMode(blindIn2, OUTPUT);

  // Initially turn everything off
  digitalWrite(relayPin, LOW);
  digitalWrite(motorEnable, LOW);
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, LOW);
  digitalWrite(blindRelayPin, LOW);
  digitalWrite(blindEnable, LOW);
  digitalWrite(blindIn1, LOW);
  digitalWrite(blindIn2, LOW);

  // Initialize WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize the BME280 sensor
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  Serial.println("BME280 sensor initialized successfully!");

  // Initialize the BH1750 sensor
  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("Could not find a valid BH1750 sensor, check wiring!");
    while (1);
  }
  Serial.println("BH1750 sensor initialized successfully!");

  // Initialize the NTP client
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Read data from the BME280 and BH1750 sensors
    float temperature = bme.readTemperature();
    float humidity = bme.readHumidity();
    float pressure = bme.readPressure() / 100.0F;
    float lux = lightMeter.readLightLevel();

    // Update time from NTP
    String timestamp = getTime();

    // Determine the current status based on conditions
    String status = "";
    if (temperature > tempHot) {
      status = "Hot";
      if (!windowOpen) openWindowSequentially();
    } else if (temperature > tempWarm && humidity >= 40 && humidity <= 70 && 
               pressure >= pressureWarmMin && pressure <= pressureWarmMax) {
      status = "Warm";
      if (!windowOpen) openWindowSequentially();
    } else if (temperature >= tempCool && temperature <= tempWarm && 
               humidity > humidityRainy && pressure < pressureRainy) {
      status = "Rainy";
      if (windowOpen) closeWindowSequentially();
    } else {
      status = "Cool";
      if (windowOpen) closeWindowSequentially();
    }

    // Control window blind based on light intensity
    if (lux < lightLow) {
      if (!blindOpen) rollUpBlindSequentially();
    } else {
      if (blindOpen) rollDownBlindSequentially();
    }

    // Output sensor data and status to Serial Monitor
    Serial.print("Temperature = ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Humidity = ");
    Serial.print(humidity);
    Serial.println(" %");

    Serial.print("Pressure = ");
    Serial.print(pressure);
    Serial.println(" hPa");

    Serial.print("Light Intensity = ");
    Serial.print(lux);
    Serial.println(" lux");

    Serial.print("Condition Status = ");
    Serial.println(status);

    Serial.print("Timestamp = ");
    Serial.println(timestamp);

    // Send sensor data and status to Firebase
    sendDataToFirebase(temperature, humidity, pressure, lux, status, timestamp);

    delay(5000); // Delay for 5 seconds before the next reading
  }
}

String getTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return "";
  }
  char timeString[25];
  strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(timeString);
}

// Function to send data to Firebase
void sendDataToFirebase(float temperature, float humidity, float pressure, float lux, String status, String timestamp) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String path = FIREBASE_HOST + "/weather.json";
    String jsonData = "{\"temperature\":" + String(temperature) +
                      ", \"humidity\":" + String(humidity) +
                      ", \"pressure\":" + String(pressure) +
                      ", \"lightIntensity\":" + String(lux) +
                      ", \"status\":\"" + status + "\"," +
                      "\"timestamp\":\"" + timestamp + "\"}";

    http.begin(path.c_str());
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.println("Error on sending POST: " + String(httpResponseCode));
    }
    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  }
}

// Sequential function to open the window
void openWindowSequentially() {
  digitalWrite(relayPin, HIGH);
  digitalWrite(motorIn1, HIGH);
  digitalWrite(motorIn2, LOW);
  digitalWrite(motorEnable, HIGH);
  Serial.println("Opening Window");
  delay(3000);  // Run motor for 3 seconds to open window partially
  stopMotor();

  delay(500); // Small delay between actions

  digitalWrite(motorIn1, HIGH);
  digitalWrite(motorIn2, LOW);
  digitalWrite(motorEnable, HIGH);
  delay(3000);  // Run motor for additional 3 seconds to fully open window
  stopMotor();

  windowOpen = true; // Update state
}

// Sequential function to close the window
void closeWindowSequentially() {
  digitalWrite(relayPin, HIGH);
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, HIGH);
  digitalWrite(motorEnable, HIGH);
  delay(3000);  // Run motor for 3 seconds to close window partially
  stopMotor();

  delay(500); // Small delay between actions

  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, HIGH);
  digitalWrite(motorEnable, HIGH);
  delay(3000);  // Run motor for additional 3 seconds to fully close window
  stopMotor();

  windowOpen = false; // Update state
}

// Sequential function to roll up the blinds
void rollUpBlindSequentially() {
  digitalWrite(blindRelayPin, HIGH);
  digitalWrite(blindIn1, HIGH);
  digitalWrite(blindIn2, LOW);
  digitalWrite(blindEnable, HIGH);
  delay(3000);  // Run motor for 3 seconds to roll up blind partially
  stopMotor();

  delay(500); // Small delay between actions

  digitalWrite(blindIn1, HIGH);
  digitalWrite(blindIn2, LOW);
  digitalWrite(blindEnable, HIGH);
  delay(3000);  // Run motor for additional 3 seconds to fully roll up blind
  stopMotor();

  blindOpen = true; // Update state
}

// Sequential function to roll down the blinds
void rollDownBlindSequentially() {
  digitalWrite(blindRelayPin, HIGH);
  digitalWrite(blindIn1, LOW);
  digitalWrite(blindIn2, HIGH);
  digitalWrite(blindEnable, HIGH);
  delay(3000);  // Run motor for 3 seconds to roll down blind partially
  stopMotor();

  delay(500); // Small delay between actions

  digitalWrite(blindIn1, LOW);
  digitalWrite(blindIn2, HIGH);
  digitalWrite(blindEnable, HIGH);
  delay(3000);  // Run motor for additional 3 seconds to fully roll down blind
  stopMotor();

  blindOpen = false; // Update state
}

// Function to stop motor
void stopMotor() {
  digitalWrite(motorEnable, LOW);
  digitalWrite(relayPin, LOW);
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, LOW);
  digitalWrite(blindEnable, LOW);
  digitalWrite(blindRelayPin, LOW);
}
