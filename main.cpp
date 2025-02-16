#include <LiquidCrystal_I2C.h>

// Initialize LCD
LiquidCrystal_I2C lcd(0x20, 16, 2);

// Sensor pins
const int tempPin = A1;        // Temperature sensor pin
const int humidityPin = A2;   // Humidity potentiometer pin
const int pressurePin = A3;   // Pressure potentiometer pin
const int gasPin = A0;        // Gas sensor pin

// LED pins
const int greenLED = 4;       // Green LED for fresh air quality
const int yellowLED = 9;      // Yellow LED for moderate air quality
const int redLED = 3;         // Red LED for bad air quality
const int buzzerPin = 6;      // Buzzer pin

// Track last activity time for power management
unsigned long lastActivity = 0;

void setup() {
  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Initialize Serial
  Serial.begin(9600);

  // Set LED and buzzer pins as outputs
  pinMode(greenLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  // Read sensor values
  float temp = readTemperature();
  int humidity = readPotentiometer(humidityPin, 0, 100);
  int pressure = readPotentiometer(pressurePin, 900, 1100);
  int AQI = analogRead(gasPin); // Read air quality sensor

  // Determine AQI status based on WHO standards
  String AQIStatus;
  if (AQI < 50) {
    AQIStatus = "Good";
    updateLEDs(greenLED);
    noTone(buzzerPin); // Turn off buzzer
  } else if (AQI < 100) {
    AQIStatus = "Moderate";
    updateLEDs(yellowLED);
    noTone(buzzerPin); // Turn off buzzer
  } else {
    AQIStatus = "Poor";
    updateLEDs(redLED);
    tone(buzzerPin, 1000, 1000); // Emit a 1kHz sound for 1000ms
  }

  // Display readings on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temp, 1);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  lcd.print("%");

  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Air Pres: ");
  lcd.print(pressure);
  lcd.print("hPa");

  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AQI: ");
  lcd.print(AQI);

  lcd.setCursor(0, 1);
  lcd.print("Air Q: ");
  lcd.print(AQIStatus);

  delay(2000); // Delay for readability

  // Reset last activity timer
  lastActivity = millis();

  // Manage power (turn off backlight if idle)
  managePower();
}

float readTemperature() {
  int reading = analogRead(tempPin);          // Read temperature sensor
  float voltage = (reading * 5.0) / 1024.0;   // Convert to voltage
  return (voltage - 0.5) * 100;               // Convert to Celsius
}

int readPotentiometer(int pin, int minVal, int maxVal) {
  return map(analogRead(pin), 0, 1023, minVal, maxVal); // Map sensor value to range
}

void updateLEDs(int activeLED) {
  // Turn off all LEDs
  digitalWrite(greenLED, LOW);
  digitalWrite(yellowLED, LOW);
  digitalWrite(redLED, LOW);

  // Turn on the active LED
  digitalWrite(activeLED, HIGH);
}

void managePower() {
  // Check if 10 seconds of inactivity has elapsed
  if (millis() - lastActivity > 10000) {
    lcd.noBacklight(); // Turn off the backlight
  } else {
    lcd.backlight();   // Keep the backlight on
  }
} 
