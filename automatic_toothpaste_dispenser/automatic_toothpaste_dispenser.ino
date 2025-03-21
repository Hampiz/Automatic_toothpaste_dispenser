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

bool dispensing = false;
bool toothbrushPresent = false;

void setup() {
    pinMode(sensorPin, INPUT);
    pinMode(buzzerPin, OUTPUT);
    pinMode(ledPin, OUTPUT);

    stepper.setSpeed(15);
    Serial.begin(9600);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        Serial.println("SSD1306 initialization failed");
        for (;;);
    }

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
        toothbrushPresent = true;  // Markera tandborste som närvarande
        Serial.println("Tandborste upptäckt, dispenserar...");
        updateDisplay("Dispenserar...");
        dispenseToothpaste();
    } 
    else if (!sensorTriggered) {
        toothbrushPresent = false;  // Återställ när tandborsten tas bort
    }
}

void dispenseToothpaste() {
    dispensing = true;
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(ledPin, HIGH);
    Serial.println("Trycker ut tandkräm...");

    for (int i = 0; i < 300; i++) {  // Steg för att trycka ut rätt mängd
        stepper.step(1);
    }

    delay(500);

    Serial.println("Drar tillbaka tryckaren...");
    
    for (int i = 0; i < 300; i++) {  // Samma antal steg tillbaka
        stepper.step(-1);
    }

    digitalWrite(buzzerPin, LOW);
    digitalWrite(ledPin, LOW);
    Serial.println("Färdig, väntar på nästa tandborste.");
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
