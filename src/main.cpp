#include <Arduino.h>
#include <FastLED.h>

#define RGB_PIN 48
#define NUM_LEDS 1
#define POT_PIN_1 4
#define POT_PIN_2 5
#define STATUS_LED 2
#define STATUS_LED 6
#define BUTTON_PIN_1 11
#define BUTTON_PIN_2 12
#define BUTTON_PIN_3 13
#define BUTTON_PIN_4 14

CRGB leds[NUM_LEDS];
uint8_t brightness = 50;
float speed = 1.0f;
bool rainbowActive = true;
float preciseHue = 0.0f;

unsigned long lastBlinkMillis = 0;
bool statusLedState = false;

void printMenu() {
    Serial.println("\n--- ESP32-S3 DEBUG CONSOLE ---");
    Serial.println("Commands:");
    Serial.println("  'p' -> Rainbow Start/Stop");
    Serial.println("  '+' -> Increase brightness");
    Serial.println("  '-' -> Decrease brightness");
    Serial.println("  'f' -> Increase speed");
    Serial.println("  's' -> Decrease speed");
    Serial.println("  'r' -> Manual LED: RED");
    Serial.println("  'g' -> Manual LED: GREEN");
    Serial.println("  'b' -> Manual LED: BLUE");
    Serial.println("  'n' -> Reset settings");
    Serial.println("------------------------------");
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    pinMode(STATUS_LED, OUTPUT);

    pinMode(BUTTON_PIN_1, INPUT_PULLUP);
    pinMode(BUTTON_PIN_2, INPUT_PULLUP);
    pinMode(BUTTON_PIN_3, INPUT_PULLUP);
    pinMode(BUTTON_PIN_4, INPUT_PULLUP);

    FastLED.addLeds<WS2812B, RGB_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(brightness);

    Serial.println("System ready!");
    printMenu();
}

void loop() {
    if (rainbowActive) {
        preciseHue += speed;
        if (preciseHue >= 255.0f) preciseHue -= 255.0f;
        if (preciseHue < 0.0f) preciseHue += 255.0f;
        leds[0] = CHSV(static_cast<uint8_t>(preciseHue), 255, 255);
    if (digitalRead(BUTTON_PIN_1) == LOW) {
        rainbowActive = false;
        leds[0] = CRGB::Red;
        FastLED.show();
        Serial.println("Manual mode: LED is RED");
        delay(200);
    }

    if (digitalRead(BUTTON_PIN_2) == LOW) {
        rainbowActive = false;
        leds[0] = CRGB::Blue;
        FastLED.show();
        Serial.println("Manual mode: LED is BLUE");
        delay(200);
    }

    if (digitalRead(BUTTON_PIN_3) == LOW) {
        rainbowActive = false;
        leds[0] = CRGB::Green;
        FastLED.show();
        Serial.println("Manual mode: LED is GREEN");
        delay(200);
    }

    if (digitalRead(BUTTON_PIN_4) == LOW) {
        rainbowActive = !rainbowActive;
        Serial.printf("Rainbow effect is now %s\n", rainbowActive ? "on" : "off");
        delay(200);
    }
    }

    if (Serial.available() > 0) {
        char incomingByte = Serial.read();

        Serial.print("Input detected: ");
        Serial.println(incomingByte);

        switch (incomingByte) {
            case 'p':
                rainbowActive = !rainbowActive;
                Serial.printf("Rainbow effect is now %s\n", rainbowActive ? "on" : "off");
                break;
            case '+':
                if (brightness < 245) brightness += 10;
                FastLED.setBrightness(brightness);
                Serial.printf("Brightness increased to %d\n", brightness);
                break;
            case '-':
                if (brightness > 10) brightness -= 10;
                FastLED.setBrightness(brightness);
                Serial.printf("Brightness decreased to %d\n", brightness);
                break;
            case 'r':
                rainbowActive = false;
                leds[0] = CRGB::Red;
                FastLED.show();
                Serial.println("Manual mode: LED is RED");
                break;
            case 'b':
                rainbowActive = false;
                leds[0] = CRGB::Blue;
                FastLED.show();
                Serial.println("Manual mode: LED is BLUE");
                break;
            case 'g':
                rainbowActive = false;
                leds[0] = CRGB::Green;
                FastLED.show();
                Serial.println("Manual mode: LED is GREEN");
                break;
            case 'f':
                speed += 0.01f;
                Serial.printf("Speed increased to %f\n", speed);
                break;
            case 's':
                speed -= 0.01f;
                Serial.printf("Speed decreased to %f\n", speed);
                break;
            case 'n':
                rainbowActive = true;
                preciseHue = 0.0f;
                brightness = 50;
                speed = 1.0f;
                Serial.println("Settings have been reset");
                break;
            case '\n':
            case '\r':
                break;
            default:
                Serial.println("Unknown command!");
                printMenu();
                break;
        }
    }

    if (rainbowActive) {
        preciseHue += speed;
        if (preciseHue >= 255.0f) preciseHue -= 255.0f;
        if (preciseHue < 0.0f) preciseHue += 255.0f;
        leds[0] = CHSV(static_cast<uint8_t>(preciseHue), 255, 255);
        FastLED.show();
    }

    unsigned long currentMillis = millis();
    if (statusLedState) {
        if (currentMillis - lastBlinkMillis >= 200) {
            lastBlinkMillis = currentMillis;
            statusLedState = false;
            digitalWrite(STATUS_LED, statusLedState);
        }
    } else {
        if (currentMillis - lastBlinkMillis >= 5000) {
            lastBlinkMillis = currentMillis;
            statusLedState = true;
            digitalWrite(STATUS_LED, statusLedState);
        }
    }

    delay(5);
}