#include <TM1637.h>
#include <Adafruit_NeoPixel.h>

#define pir 2
#define button 4
#define buzzer 3
#define neopixel 5
#define clk 27
#define dio 28

bool alarmTriggered = false;
bool systemLocked = false;
unsigned long alarmStartTime = 0;
unsigned long lockoutStartTime = 0;
int passcode[] = {3, 1, 4, 2};
int enteredCode[4] = {0, 0, 0, 0};
int codeIndex = 0;
int attempts = 0;
const int maxAttempts = 3;
const unsigned long lockoutDuration = 30000;
unsigned long lastButtonPressTime = 0;
const unsigned long buttonDebounceDelay = 200;

TM1637 tm(clk, dio);
Adafruit_NeoPixel pixels(1, neopixel, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(pir, INPUT);
  pinMode(button, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  pinMode(neopixel, OUTPUT);
  tm.init();
  tm.set(BRIGHT_TYPICAL);
  pixels.begin();
  pixels.clear();
}

void loop() {
  if (systemLocked) {
    if (millis() - lockoutStartTime > lockoutDuration) {
      systemLocked = false;
      attempts = 0;
      tm.clearDisplay();
    } else {
      return;
    }
  }

  if (digitalRead(pir) == HIGH && !alarmTriggered) {
    alarmTriggered = true;
    alarmStartTime = millis();
  }

  if (alarmTriggered) {
    digitalWrite(buzzer, HIGH);
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.show();

    unsigned long elapsedSeconds = (millis() - alarmStartTime) / 1000;
    tm.display(0, (elapsedSeconds / 100) % 10);
    tm.display(1, (elapsedSeconds / 10) % 10);
    tm.display(2, elapsedSeconds % 10);
  }

  if (digitalRead(button) == LOW && millis() - lastButtonPressTime > buttonDebounceDelay) {
    lastButtonPressTime = millis();
    enteredCode[codeIndex]++;
    if (enteredCode[codeIndex] > 9) enteredCode[codeIndex] = 0;
    tm.display(codeIndex, enteredCode[codeIndex]);
  }

  if (digitalRead(button) == HIGH && millis() - lastButtonPressTime > 1000 && enteredCode[codeIndex] > 0) {
    lastButtonPressTime = millis();
    codeIndex++;
    if (codeIndex >= 4) {
      if (checkPasscode()) {
        resetAlarm();
      } else {
        attempts++;
        if (attempts >= maxAttempts) {
          systemLocked = true;
          lockoutStartTime = millis();
          pixels.setPixelColor(0, pixels.Color(0, 0, 255));
          pixels.show();
          tm.clearDisplay();
        } else {
          resetEnteredCode();
        }
      }
    }
  }
}

bool checkPasscode() {
  for (int i = 0; i < 4; i++) {
    if (enteredCode[i] != passcode[i]) {
      return false;
    }
  }
  return true;
}

void resetEnteredCode() {
  for (int i = 0; i < 4; i++) {
    enteredCode[i] = 0;
    tm.display(i, 0);
  }
  codeIndex = 0;
}

void resetAlarm() {
  alarmTriggered = false;
  digitalWrite(buzzer, LOW);
  pixels.clear();
  pixels.show();
  tm.clearDisplay();
  resetEnteredCode();
  attempts = 0;
}
