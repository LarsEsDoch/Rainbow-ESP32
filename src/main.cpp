#include <Arduino.h>
#include <FastLED.h>

#define RGB_PIN 48
#define NUM_LEDS 1
#define POT_PIN_1 4
#define POT_PIN_2 5
#define STATUS_LED 2
#define BUTTON_PIN_1 11
#define BUTTON_PIN_2 12
#define BUTTON_PIN_3 13
#define BUTTON_PIN_4 14

CRGB leds[NUM_LEDS];
uint8_t brightness = 50;
float speed = 1.0f;
bool rainbowActive = true;
float preciseHue = 0.0f;

void printMenu() {
    Serial.println("\n--- ESP32-S3 DEBUG KONSOLE ---");
    Serial.println("Befehle:");
    Serial.println("  'p' -> Regenbogen Start/Stop");
    Serial.println("  '+' -> Helligkeit erhöhen");
    Serial.println("  '-' -> Helligkeit senken");
    Serial.println("  'f' -> Geschwindigkeit erhöhen");
    Serial.println("  's' -> Geschwindigkeit senken");
    Serial.println("  'r' -> LED manuell auf ROT");
    Serial.println("  'g' -> LED manuell auf GRÜN");
    Serial.println("  'b' -> LED manuell auf BLAU");
    Serial.println("  'n' -> Einstellung reset");
    Serial.println("------------------------------");
}

void setup() {
    Serial.begin(115200);
    delay(2000);


    FastLED.addLeds<WS2812B, RGB_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(brightness);

    Serial.println("System bereit!");
    printMenu();
}

void loop() {
    if (rainbowActive) {
        preciseHue += speed;
        if (preciseHue >= 255.0f) preciseHue -= 255.0f;
        if (preciseHue < 0.0f) preciseHue += 255.0f;
        leds[0] = CHSV(static_cast<uint8_t>(preciseHue), 255, 255);
        FastLED.show();
    }

    if (Serial.available() > 0) {
        char incomingByte = Serial.read();

        Serial.print("Eingabe empfangen: ");
        Serial.println(incomingByte);

        switch (incomingByte) {
            case 'p':
                rainbowActive = !rainbowActive;
                Serial.printf("Regenbogen ist jetzt: %s\n", rainbowActive ? "AN" : "AUS");
                break;
            case '+':
                if (brightness < 245) brightness += 10;
                FastLED.setBrightness(brightness);
                Serial.printf("Helligkeit erhöht auf: %d\n", brightness);
                break;
            case '-':
                if (brightness > 10) brightness -= 10;
                FastLED.setBrightness(brightness);
                Serial.printf("Helligkeit gesenkt auf: %d\n", brightness);
                break;
            case 'r':
                rainbowActive = false;
                leds[0] = CRGB::Red;
                FastLED.show();
                Serial.println("Manueller Modus: LED ist ROT");
                break;
            case 'b':
                rainbowActive = false;
                leds[0] = CRGB::Blue;
                FastLED.show();
                Serial.println("Manueller Modus: LED ist BLAU");
                break;
            case 'g':
                rainbowActive = false;
                leds[0] = CRGB::Green;
                FastLED.show();
                Serial.println("Manueller Modus: LED ist GRÜN");
                break;
            case 'f':
                speed += 0.01f;
                Serial.printf("Speed erhöht auf: %f\n", speed);
                break;
            case 's':
                speed -= 0.01f;
                Serial.printf("Speed verringert auf: %f\n", speed);
                break;
            case 'n':
                rainbowActive = true;
                preciseHue = 0.0f;
                brightness = 50;
                speed = 1.0f;
                Serial.println("Einstellung wurden zurück gesetzt");
                break;
            case '\n':
            case '\r':
                break;
            default:
                Serial.println("Unbekannter Befehl!");
                printMenu();
                break;
        }
    }

    delay(5);
}