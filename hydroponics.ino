#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <FirebaseESP8266HTTPClient.h>

// WiFi credentials
const char* ssid = "WiFiSSID";
const char* password = "WiFiPassword";

// Firebase credentials
#define FIREBASE_HOST "firebase-host.firebaseio.com"
#define FIREBASE_AUTH "firebase-authentication"

// Threshold values
const float pH_threshold = 6.5;
const int light_intensity_threshold = 1000;
const float temperature_threshold = 25.0;

// Actuator pins
const int pump_acid_1 = D1;
const int pump_acid_2 = D2;
const int pump_base_1 = D3;
const int pump_base_2 = D4;
const int pump_hot_water = D5;
const int pump_cold_water = D6;

// Sensor pins
const int pH_sensor_pin = A0;
const int light_sensor_pin = A1;
const int temperature_sensor_pin = A2;

// Firebase objects
FirebaseData firebaseData;
FirebaseJson json;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
  
  // Initialize Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  
  // Initialize actuator pins
  pinMode(pump_acid_1, OUTPUT);
  pinMode(pump_acid_2, OUTPUT);
  pinMode(pump_base_1, OUTPUT);
  pinMode(pump_base_2, OUTPUT);
  pinMode(pump_hot_water, OUTPUT);
  pinMode(pump_cold_water, OUTPUT);
}

void loop() {
  // Read sensor values
  float pH_value = analogRead(pH_sensor_pin) * 5.0 / 1023.0;
  int light_intensity = analogRead(light_sensor_pin);
  float temperature = analogRead(temperature_sensor_pin) * 0.48875;
  
  // Upload sensor values to Firebase
  json.clear();
  json.add("pH", pH_value);
  json.add("light_intensity", light_intensity);
  json.add("temperature", temperature);
  
  Firebase.set(firebaseData, "/sensors", json);
  
  // Check if any threshold is breached
  if (pH_value < pH_threshold) {
    activatePumps(pump_acid_1, pump_acid_2);
  }
  
  if (pH_value > pH_threshold) {
    activatePumps(pump_base_1, pump_base_2);
  }
  
  if (light_intensity > light_intensity_threshold) {
    activatePumps(pump_hot_water);
  }
  
  if (temperature > temperature_threshold) {
    activatePumps(pump_cold_water);
  }
  
  delay(5000);  // Wait for 5 seconds before repeating the loop
}

void activatePumps(int pin1, int pin2 = -1) {
  digitalWrite(pin1, HIGH);
  if (pin2 != -1) {
    digitalWrite(pin2, HIGH);
  }
  
  delay(5000);  // Run the pumps for 5 seconds
  
  digitalWrite(pin1, LOW);
  if (pin2 != -1) {
    digitalWrite(pin2, LOW);
  }
}

