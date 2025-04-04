#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Stepper.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define STEPS 2048  // Steps per revolution for stepper motor
Stepper stepper(STEPS, 8, 10, 9, 11);

const int sensorPin = 2;
const int buzzerPin = 4;
const int ledPin = 3;
const int potentiometerPin = A0;

// Motor driver pins for power control
const int motorPowerPin = 7;

bool dispensing = false;
bool toothbrushPresent = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;  // Debounce time in milliseconds
int lastPotValue = 0;
int currentPotValue = 0;

void setup() {
    pinMode(sensorPin, INPUT_PULLUP);  // Enable internal pull-up resistor
    pinMode(buzzerPin, OUTPUT);
    pinMode(ledPin, OUTPUT);
    pinMode(potentiometerPin, INPUT);
    pinMode(motorPowerPin, OUTPUT);
    digitalWrite(motorPowerPin, LOW);  // Start with motor power off
    
    stepper.setSpeed(15);
    Serial.begin(9600);
    
    // Initialize display with proper error checking
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        Serial.println(F("SSD1306 allocation failed"));
        // Basic error indication with LED
        for(;;) {
            digitalWrite(ledPin, HIGH);
            delay(300);
            digitalWrite(ledPin, LOW);
            delay(300);
        }
    }
    
    // Display welcome message
    updateDisplay("Ready for toothpaste!");
    
    // Initial potentiometer reading
    lastPotValue = analogRead(potentiometerPin);
}

void loop() {
    // Read sensor state with debouncing
    static bool lastSensorState = HIGH;
    bool currentSensorState = digitalRead(sensorPin);
    
    if (currentSensorState != lastSensorState) {
        lastDebounceTime = millis();
    }
    
    // Debounced sensor reading
    if ((millis() - lastDebounceTime) > debounceDelay) {
        // Toothbrush detected (sensor LOW)
        if (currentSensorState == LOW && !toothbrushPresent && !dispensing) {
            toothbrushPresent = true;
            updateDisplay("Dispensing...");
            dispenseToothpaste();
        } 
        else if (currentSensorState == HIGH) {
            toothbrushPresent = false;
        }
    }
    
    lastSensorState = currentSensorState;
    
    // Read potentiometer and update display if value changed significantly
    currentPotValue = analogRead(potentiometerPin);
    if (abs(currentPotValue - lastPotValue) > 20 && !dispensing) {  // Ignore small changes
        lastPotValue = currentPotValue;
        int amount = map(currentPotValue, 0, 1023, 10, 100);
        
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(10, 10);
        display.println("Ready for toothpaste!");
        display.setCursor(10, 30);
        display.print("Amount: ");
        display.print(amount);
        display.println("%");
        
        // Draw a visual bar indicator
        display.drawRect(10, 45, 108, 10, WHITE);
        display.fillRect(10, 45, map(amount, 10, 100, 10, 108), 10, WHITE);
        display.display();
    }
    
    // Power saving: ensure stepper motor is off when not in use
    if (!dispensing) {
        digitalWrite(motorPowerPin, LOW);
    }
}

void dispenseToothpaste() {
    dispensing = true;
    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    delay(100);  // Short beep
    digitalWrite(buzzerPin, LOW);
    
    // Power on the motor
    digitalWrite(motorPowerPin, HIGH);
    delay(50);  // Give time for motor to power up
    
    // Map potentiometer value to steps (with reasonable limits)
    int steps = map(analogRead(potentiometerPin), 0, 1023, 100, 500);
    
    // Forward movement - push toothpaste
    stepper.step(steps);
    delay(200); // Short pause
    
    // Reverse to prevent dripping
    stepper.step(-40);  // Shorter retraction to prevent pulling toothpaste back
    
    // Signal completion
    digitalWrite(buzzerPin, HIGH);
    delay(100);
    digitalWrite(buzzerPin, LOW);
    delay(100);
    digitalWrite(buzzerPin, HIGH);
    delay(100);
    digitalWrite(buzzerPin, LOW);
    
    digitalWrite(ledPin, LOW);
    updateDisplay("Done! Move toothbrush.");
    
    // Power off the motor
    digitalWrite(motorPowerPin, LOW);
    dispensing = false;
    
    // Wait for toothbrush to be removed
    delay(500);
}

void updateDisplay(String message) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(10, 10);
    display.println(message);
    display.display();
}