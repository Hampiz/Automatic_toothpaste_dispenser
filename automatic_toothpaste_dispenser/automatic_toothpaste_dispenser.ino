#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Stepper.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define STEPS 2048  // Steg per varv för stegmotorn
Stepper stepper(STEPS, 8, 10, 9, 11);

const int sensorPin = 2;
const int buzzerPin = 4;
const int ledPin = 3;
const int potentiometerPin = A0;

bool dispensing = false;
bool toothbrushPresent = false;

void setup() {
    pinMode(sensorPin, INPUT);
    pinMode(buzzerPin, OUTPUT);
    pinMode(ledPin, OUTPUT);
    pinMode(potentiometerPin, INPUT);

    stepper.setSpeed(15);
    Serial.begin(9600);

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(10, 10);
    display.println("Redo för tandkräm!");
    display.display();
}

void loop() {
    bool sensorTriggered = digitalRead(sensorPin) == LOW;  // Tandborste upptäckt

    if (sensorTriggered && !toothbrushPresent) {
        toothbrushPresent = true;  
        updateDisplay("Dispenserar...");
        dispenseToothpaste();
    } 
    else if (!sensorTriggered) {
        toothbrushPresent = false;
    }
}

void dispenseToothpaste() {
    dispensing = true;
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(ledPin, HIGH);
    
    // Läs potentiometern och mappa värdet till antal steg
    int steps = map(analogRead(potentiometerPin), 0, 1023, 100, 500);

    for (int i = 0; i < steps; i++) {
        stepper.step(1);
    }

    delay(500);
    
    for (int i = 0; i < steps; i++) {  
        stepper.step(-1);
    }

    digitalWrite(buzzerPin, LOW);
    digitalWrite(ledPin, LOW);
    updateDisplay("Färdig! Flytta tandborsten.");
    dispensing = false;
}

void updateDisplay(String message) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(10, 10);
    display.println(message);
    display.display();
}
