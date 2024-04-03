#include <ESP8266WiFi.h>
#include <Wire.h>
#include <NewPing.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ThingSpeak.h>

#define TRIGGER_PIN D5      // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN D6         // Arduino pin tied to echo pin on the ultrasonic sensor.
#define BUZZER_PIN D7       // Arduino pin tied to the buzzer
#define ONE_WIRE_BUS D4     // Arduino pin tied to the one-wire bus for DS18B20 temperature sensor
#define LED_GREEN D8        // Arduino pin tied to the green LED
#define LED_RED1 D3          // Arduino pin tied to the red LED
#define LED_RED2 D1
#define MIN_TEMP 25         // Minimum temperature threshold for green LED
#define MAX_TEMP 47         // Maximum temperature threshold for red LED

const char* ssid = "Amrita_CHN";
const char* password = "amrita@321";

const long channelID = 2491635;
const char* apikey = "WHIRKOVSKTUGT5SK";

NewPing sonar(TRIGGER_PIN, ECHO_PIN, 200); // NewPing setup of pins and maximum distance.
OneWire oneWire(ONE_WIRE_BUS);             // Setup a oneWire instance to communicate with DS18B20 temperature sensor
DallasTemperature sensors(&oneWire);       // Pass oneWire reference to Dallas Temperature sensor

const int PulseSensorHRWire = A0;  // PulseSensor connected to ANALOG PIN 0 (A0 / ADC 0).
int Threshold = 600;               // Determine which Signal to "count as a beat" and which to ignore.
int Signal;

WiFiClient client;

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED1, OUTPUT);
  pinMode(LED_RED2, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status()!=WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected.");

  ThingSpeak.begin(client);

  // Initialize DS18B20 temperature sensor
  sensors.begin();
}

void loop() {
  // Read distance from ultrasonic sensor
  int distance = sonar.ping_cm();

  // Check if the distance is valid (not 0, which might indicate an issue)
  if (distance != 0) {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // Check if distance exceeds 100cm
    if (distance <= 100) {
      digitalWrite(BUZZER_PIN, HIGH); // Activate buzzer if distance exceeds 100cm
    } else {
      digitalWrite(BUZZER_PIN, LOW);  // Deactivate buzzer if distance is less than 100cm
    }
  } else {
    Serial.println("Error: Invalid distance reading");
  }

  // Read temperature from DS18B20 sensor
  sensors.requestTemperatures(); // Request temperature reading
  float temperatureC = sensors.getTempCByIndex(0); // Get temperature in Celsius
  temperatureC = abs (((temperatureC / 102.0) * 1.0) - 0.25) * 100.0;
  // Print temperature reading
  Serial.print("Temperature: ");
  Serial.print(temperatureC);
  Serial.println(" C");

  // Check if temperature is within the specified range
  if (temperatureC >= MIN_TEMP && temperatureC <= MAX_TEMP) {
    // Temperature within range
    digitalWrite(LED_GREEN, HIGH);  // Turn on green LED
    digitalWrite(LED_RED1, LOW);      // Turn off red LED
  } else {
    // Temperature out of range
    digitalWrite(LED_GREEN, LOW);   // Turn off green LED
    digitalWrite(LED_RED1, HIGH);     // Turn on red LED
  }
  
  Signal = analogRead(PulseSensorHRWire);
  // Serial.println(Signal);
  // int mappedValue = 0;
 

  
  int mappedValue = map(Signal, 0, 1023, 80, 130); 
    Serial.print("Pulse : "); 
    Serial.println(mappedValue);
    if(mappedValue >= 110) {
      digitalWrite(LED_RED2, HIGH); // Turn on the second red LED if pulse is greater than 120
    } else {
       
      digitalWrite(LED_RED2, LOW);
       // Turn off the second red LED if pulse is less than or equal to 120
    }
 
 
 ThingSpeak.writeField(channelID, 2, temperatureC, apikey);
 ThingSpeak.writeField(channelID, 1, distance, apikey);
 ThingSpeak.writeField(channelID, 3, mappedValue, apikey);

  delay(2000); // Delay before next reading
}


