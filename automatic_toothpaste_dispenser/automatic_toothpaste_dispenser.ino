#include <Stepper.h>

#define STEPS 2048  // Steps per revolution for 28BYJ-48
Stepper stepper(STEPS, 8, 10, 9, 11);

const int sensorPin = 2;     // Obstacle avoidance sensor (toothbrush detection)
const int limitSwitchPin = 3; // Bottom limit switch

bool dispensing = false;
bool toothbrushPresent = false;

void setup() {
    pinMode(sensorPin, INPUT);
    pinMode(limitSwitchPin, INPUT_PULLUP);  // Enable internal pull-up resistor
    stepper.setSpeed(10);
    Serial.begin(9600);
}

void loop() {
    bool sensorTriggered = digitalRead(sensorPin) == LOW;  // Toothbrush detected

    if (sensorTriggered && !toothbrushPresent) {
        toothbrushPresent = true;  // Mark toothbrush as present
        Serial.println("Toothbrush detected, dispensing...");
        dispenseToothpaste();
    } 
    else if (!sensorTriggered) {
        toothbrushPresent = false;  // Reset when toothbrush is removed
    }
}

void dispenseToothpaste() {
    dispensing = true;

    // Move forward to dispense
    for (int i = 0; i < 300; i++) {  // Adjust step count for perfect amount
        stepper.step(1);
    }

    delay(500);  // Small delay before retracting

    // Retract toothpaste presser
    for (int i = 0; i < 300; i++) {
        stepper.step(-1);
    }

    Serial.println("Dispensing complete, waiting for next toothbrush.");
    dispensing = false;
}
