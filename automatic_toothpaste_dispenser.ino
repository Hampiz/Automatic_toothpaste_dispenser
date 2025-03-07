#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Stepper.h>

// OLED inställningar
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Stegmotor inställningar
#define STEPS 200  // Antal steg per varv
Stepper motor(STEPS, 8, 9, 10, 11); // Motor pins
// Sensor & utgångar
const int trigPin = 6;  // FN
const int echoPin = 7;  // OUT
const int ledPin = 3;   // LED
const int buzzer = 4;   // Buzzer

void setup() {
    Serial.begin(9600);

    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(ledPin, OUTPUT);
    pinMode(buzzer, OUTPUT);
    motor.setSpeed(15); // Justera motorhastigheten

    // Initiera OLED-skärm
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  
        Serial.println("OLED-skärm saknas!");
        for (;;); // Fastna här om ingen skärm hittas
    }
    
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(10, 20);
    display.print("Vantar...");
    display.display();
}

void loop() {
    // Mät avstånd med sensorn
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    int distance = duration * 0.034 / 2;  // Omvandla tid till cm

    if (distance < 10) {  // Om en tandborste är närmare än 10 cm
        // Uppdatera OLED-skärm
        display.clearDisplay();
        display.setCursor(10, 10);
        display.print("Tandborste");
        display.setCursor(10, 30);
        display.print("upptackt!");
        display.display();

        delay(500);

        // Aktivera LED & summer
        digitalWrite(ledPin, HIGH);
        tone(buzzer, 1000, 200);

        // Uppdatera OLED-skärm till dispenseringsläge
        display.clearDisplay();
        display.setCursor(10, 20);
        display.print("Tandkram");
        display.setCursor(10, 40);
        display.print("dispenseras!");
        display.display();

        // Tryck ut tandkräm
        motor.step(100);  

        delay(500);  // Kort paus
        motor.step(-100);  // Återställ motorn

        digitalWrite(ledPin, LOW);

        // Återgå till vänteläge
        display.clearDisplay();
        display.setCursor(10, 20);
        display.print("Vantar...");
        display.display();
    }

    delay(500);
}
