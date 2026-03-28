#include <Arduino.h>
#include <FastLED.h>

#define RGB_PIN 48
#define NUM_LEDS 1

#define STATUS_LED 6

#define BUTTON_PIN_1 10
#define BUTTON_PIN_2 11
#define BUTTON_PIN_3 12
#define BUTTON_PIN_4 13
#define BUTTON_PIN_5 14

#define POT_PIN_1 4
#define POT_PIN_2 5

#define LDR_PIN 7

CRGB leds[NUM_LEDS];
int brightness = 127;
float speed = 1.0f;

bool rainbowActive = true;
bool discoActive = false;
bool ledOn = true;
bool nightMode = false;

bool debugMode = false;

float preciseHue = 0.0f;
CRGB targetColor = CRGB::Black;

unsigned long lastBlinkMillis = 0;
bool statusLedState = false;

unsigned long lastActionMillis = 0;
bool actionDetected = false;

void triggerStatusBlink() {
    lastActionMillis = millis();
    actionDetected = true;
}

unsigned long lastButton1Millis = 0;
unsigned long lastButton2Millis = 0;
unsigned long lastButton3Millis = 0;
unsigned long lastButton4Millis = 0;
unsigned long lastButton5Millis = 0;
const unsigned long debounceDelay = 200;

void printMenu() {
    Serial.println("\n--- ESP32-S3 DEBUG CONSOLE ---");
    Serial.println("=== SYSTEM ===");
    Serial.println("  'o' -> Power on/off");
    Serial.println("  'x' -> Reset settings");
    Serial.println("  'i' -> Show Debug Info");

    Serial.println("\n=== MODES ===");
    Serial.println("  'm' -> Rainbow Mode Start/Stop");
    Serial.println("  'd' -> Random Disco Color");
    Serial.println("  'n' -> Night Mode On/Off");

    Serial.println("\n=== MANUAL COLORS ===");
    Serial.println("  'r' -> RED");
    Serial.println("  'g' -> GREEN");
    Serial.println("  'b' -> BLUE");
    Serial.println("  'w' -> WHITE");

    Serial.println("\n=== ADJUSTMENTS ===");
    Serial.println("  '+' -> Increase brightness");
    Serial.println("  '-' -> Decrease brightness");
    Serial.println("  'f' -> Increase speed (faster)");
    Serial.println("  's' -> Decrease speed (slower)");
    Serial.println("------------------------------");
}

void setup() {
    Serial.begin(921600);
    delay(2000);

    pinMode(STATUS_LED, OUTPUT);

    pinMode(BUTTON_PIN_1, INPUT_PULLUP);
    pinMode(BUTTON_PIN_2, INPUT_PULLUP);
    pinMode(BUTTON_PIN_3, INPUT_PULLUP);
    pinMode(BUTTON_PIN_4, INPUT_PULLUP);
    pinMode(BUTTON_PIN_5, INPUT_PULLUP);

    FastLED.addLeds<WS2812B, RGB_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(brightness);

    Serial.println("System ready!");
    printMenu();
}

void loop() {
    if (digitalRead(BUTTON_PIN_3) == LOW) {
        if (ledOn) {
            ledOn = false;
            Serial.println("System is now off.");

            for (int i = brightness; i >= 0; i--) {
                FastLED.setBrightness(i);
                FastLED.show();
                delay(5);
            }

            analogWrite(STATUS_LED, 255);
            delay(2000);
            analogWrite(STATUS_LED, 0);
        } else {
            ledOn = true;
            Serial.println("System is now on!");

            for (int i = 0; i <= brightness; i++) {
                FastLED.setBrightness(i);
                FastLED.show();
                delay(5);
            }
        }
    }

    if (!ledOn) {
        delay(5);
        return;
    }

    static int staticColor = 0;
    if (digitalRead(BUTTON_PIN_1) == LOW) {
        rainbowActive = false;
        discoActive = false;
        if (staticColor == 0) {
            Serial.println("Manual mode: LED is RED");
            staticColor = 1;
        } else if (staticColor == 1) {
            Serial.println("Manual mode: LED is GREEN");
            staticColor = 2;
        } else if (staticColor == 2) {
            Serial.println("Manual mode: LED is BLUE");
            staticColor = 3;
        } else if (staticColor == 3) {
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

        Serial.printf("Random color: %d and brightness: %d\n", randomHue, brightness);
        delay(200);
    }
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
void loop() {
    if (debugMode) {
        Serial.printf(
            "MS:%8lu | ON:%c | MODE:%c%c%c | SPD:%5.2f | MBR:%3d | BR:%3d | P1:%4d(%c) | P2:%4d(%c) | LDR:%4d | RGB:%3d,%3d,%3d | ACT:%c\n",
            millis(),
            ledOn ? 'Y' : 'N',
            rainbowActive ? 'R' : '.',
            discoActive   ? 'D' : '.',
            nightMode     ? 'N' : '.',
            speed,
            manualBrightness,
            brightness,
            lastPot1Value,
            pot1Locked    ? 'L' : '.',
            lastPot2Value,
            pot2Locked    ? 'L' : '.',
            analogRead(LDR_PIN),
            leds[0].r, leds[0].g, leds[0].b,
            actionDetected ? '!' : '.'
        );
    }

    if (newBrightness != brightness && !discoActive) {
        brightness = newBrightness;
        FastLED.setBrightness(brightness);
            triggerStatusBlink();
    }

    if (Serial.available() > 0) {
        char incomingByte = Serial.read();

        Serial.print("Input detected: ");
        Serial.println(incomingByte);

        switch (incomingByte) {
            case 'm':
                triggerStatusBlink();
                rainbowActive = !rainbowActive;
                Serial.printf("Rainbow effect is now %s\n", rainbowActive ? "on" : "off");
                break;
                triggerStatusBlink();
                triggerStatusBlink();
            case '+':
                if (brightness < 245) brightness += 10;
                triggerStatusBlink();
                FastLED.setBrightness(brightness);
                Serial.printf("Brightness increased to %d\n", brightness);
                break;
            case '-':
                if (brightness > 10) brightness -= 10;
                triggerStatusBlink();
                FastLED.setBrightness(brightness);
                Serial.printf("Brightness decreased to %d\n", brightness);
                break;
            case 'r':
                triggerStatusBlink();
                rainbowActive = false;
                FastLED.show();
                Serial.println("Manual mode: LED is RED");
                break;
            case 'b':
                triggerStatusBlink();
                rainbowActive = false;
                targetColor = CRGB::Blue;
                FastLED.show();
                Serial.println("Manual mode: LED is BLUE");
                break;
            case 'g':
                triggerStatusBlink();
                rainbowActive = false;
                targetColor = CRGB::Green;
                FastLED.show();
                Serial.println("Manual mode: LED is GREEN");
                break;
                triggerStatusBlink();
            case 'f':
                speed += 0.01f;
                triggerStatusBlink();
                Serial.printf("Speed increased to %f\n", speed);
                break;
            case 's':
                speed -= 0.01f;
                triggerStatusBlink();
                Serial.printf("Speed decreased to %f\n", speed);
                break;
            case 'n':
                rainbowActive = true;
                preciseHue = 0.0f;
                brightness = 50;
                brightness = 127;
                speed = 1.0f;
                Serial.println("Settings have been reset");
                break;
            case 'i': {
                debugMode = !debugMode;
                Serial.printf("Debug mode is now %s\n", debugMode ? "on" : "off");
                break;
            }
            case '\n':
            case '\r':
                break;
            default:
                Serial.println("Unknown command!");
                printMenu();
                break;
        }
    }

            triggerStatusBlink();
                targetColor = CRGB::Red;
                targetColor = CRGB::Green;
                targetColor = CRGB::Blue;
                targetColor = CRGB::White;
            triggerStatusBlink();
            triggerStatusBlink();
            targetColor = CRGB::Black;
    if (digitalRead(BUTTON_PIN_5) == LOW) {
        if (millis() - lastButton5Millis > debounceDelay) {
            lastButton5Millis = millis();
            triggerStatusBlink();
            nightMode = !nightMode;
            Serial.printf("Night mode is now %s\n", nightMode ? "on" : "off");
        }
    }
    if (nightMode) {
        static float smoothedLDR = -1;
        int lightRaw = analogRead(LDR_PIN);
        if (smoothedLDR < 0) smoothedLDR = lightRaw;

        smoothedLDR = (smoothedLDR * 0.95f) + (lightRaw * 0.05f);

        int constrainedLDR = constrain(smoothedLDR, 0, 600);

        uint8_t dynamicBrightness = map(constrainedLDR, 0, 600, 5, 255);

        if (abs(dynamicBrightness - brightness) > 3) {
            brightness = dynamicBrightness;
            FastLED.setBrightness(brightness);
        }
    }

    if (rainbowActive) {
        preciseHue += speed;
        if (preciseHue >= 255.0f) preciseHue -= 255.0f;
        if (preciseHue < 0.0f) preciseHue += 255.0f;
        leds[0] = CHSV(static_cast<uint8_t>(preciseHue), 255, 255);
    } else if (!discoActive && ledOn && (targetColor == CRGB::Red || targetColor == CRGB::Green || targetColor == CRGB::Blue || targetColor == CRGB::White)) {
        nblend(leds[0], targetColor, 16);
    }

    unsigned long currentMillis = millis();

    if (actionDetected) {
        analogWrite(STATUS_LED, 30);
        if (currentMillis - lastActionMillis >= 30) {
            actionDetected = false;
            analogWrite(STATUS_LED, 0);
        }
    } else {
        if (statusLedState) {
            if (currentMillis - lastBlinkMillis >= 200) {
                lastBlinkMillis = currentMillis;
                statusLedState = false;
                analogWrite(STATUS_LED, 0);
            }
        } else {
            if (currentMillis - lastBlinkMillis >= 10000) {
                lastBlinkMillis = currentMillis;
                statusLedState = true;
                analogWrite(STATUS_LED, constrain(brightness, 10, 255));
            }
        }
    }

    FastLED.show();
    delay(5);
}