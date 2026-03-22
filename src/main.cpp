#include <Arduino.h>
#include <FastLED.h>

#define RGB_PIN 48
#define NUM_LEDS 1

#define STATUS_LED 6

#define BUTTON_PIN_1 11
#define BUTTON_PIN_2 12
#define BUTTON_PIN_3 13
#define BUTTON_PIN_4 14

#define POT_PIN_1 4
#define POT_PIN_2 5

CRGB leds[NUM_LEDS];
uint8_t brightness = 127;
float speed = 1.0f;
bool rainbowActive = true;
bool discoActive = false;
bool ledOn = true;
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
    if (digitalRead(BUTTON_PIN_1) == LOW) {
        rainbowActive = false;
        discoActive = false;
        if (staticColor == 0) {
            leds[0] = CRGB::Red;
            Serial.println("Manual mode: LED is RED");
            staticColor = 1;
        } else if (staticColor == 1) {
            leds[0] = CRGB::Green;
            Serial.println("Manual mode: LED is GREEN");
            staticColor = 2;
        } else if (staticColor == 2) {
            leds[0] = CRGB::Blue;
            Serial.println("Manual mode: LED is BLUE");
            staticColor = 3;
        } else if (staticColor == 3) {
            leds[0] = CRGB::White;
            Serial.println("Manual mode: LED is WHITE");
            staticColor = 0;
        }

        delay(200);
    }

    if (digitalRead(BUTTON_PIN_2) == LOW) {
        rainbowActive = false;
        discoActive = true;

        uint8_t randomHue = random(0, 256);

        brightness = random(5, 256);

        leds[0] = CHSV(randomHue, 255, 255);
        FastLED.setBrightness(brightness);

        Serial.printf("Random color: %d and brightness: %d\n", randomHue, brightness);
        delay(200);
    }

    if (digitalRead(BUTTON_PIN_4) == LOW) {
        rainbowActive = !rainbowActive;
        discoActive = false;
        Serial.printf("Rainbow effect is now %s\n", rainbowActive ? "on" : "off");
        delay(200);
    }

    static float smoothedPot1 = 0;
    static float smoothedPot2 = 0;

    int raw1 = analogRead(POT_PIN_1);
    int raw2 = analogRead(POT_PIN_2);

    smoothedPot1 = (smoothedPot1 * 0.9f) + (raw1 * 0.1f);
    smoothedPot2 = (smoothedPot2 * 0.9f) + (raw2 * 0.1f);

    float newSpeed = 0.01f + ((int)smoothedPot1 / 4095.0f) * 10.00f;
    uint8_t newBrightness = map((int)smoothedPot2, 0, 4095, 0, 255);

    if (abs(newSpeed - speed) > 0.01f) {
        speed = newSpeed;
    }

    if (newBrightness != brightness && !discoActive) {
        brightness = newBrightness;
        FastLED.setBrightness(brightness);
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
    }

    unsigned long currentMillis = millis();
    if (statusLedState) {
        if (currentMillis - lastBlinkMillis >= 200) {
            lastBlinkMillis = currentMillis;
            statusLedState = false;
            analogWrite(STATUS_LED, statusLedState);
        }
    } else {
        if (currentMillis - lastBlinkMillis >= 5000) {
            lastBlinkMillis = currentMillis;
            statusLedState = true;
            analogWrite(STATUS_LED, constrain(brightness, 10, 255));
        }
    }

    FastLED.show();
    delay(5);
}