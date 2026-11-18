/*===========================================================
                 LED MODE CONTROLLER (ESP32)
============================================================
Modes:  1 = BLINK   2 = RUN   3 = ALT   (0 = NONE, all LEDs off)
(the mode functions are in LED_Modes.h)

The mode is chosen from the Java GUI or the 2 switches,
and shown on the LCD.

SWITCHES (SW1 SW2, 1 = switch ON):
  1 1  ->  RUN
  1 0  ->  ALT
  0 1  ->  BLINK
  0 0  ->  all LEDs OFF (mode NONE)

LCD (I2C):  SDA = GPIO 21,  SCL = GPIO 22

GUI -> ESP32 :  '1' BLINK, '2' RUN, '3' ALT, '?' = ask the mode
ESP32 -> GUI :  "MODE:n"   (n = 0 NONE, 1 BLINK, 2 RUN, 3 ALT)

Programmer:
  Julian Philip D. Cientos
  Chrischele T. Palacios
-----------------------------------------------------------  */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---------------------------------------------------------
// PINS  (these must come BEFORE the #include "LED_Modes.h")
// ---------------------------------------------------------

// LEDs in order: GREEN, YELLOW, RED | GREEN, YELLOW, RED
int LED[] = {32, 33, 25, 14, 27, 26};
const int NUM_LEDS = 6;
const int HALF = 3;                  // first half = LEDs 0-2, second half = LEDs 3-5

int SW1 = 18;
int SW2 = 19;

// How are the switches wired?
//   Switch sends 3.3V when ON:  SWITCH_ON = HIGH, SWITCH_PIN_MODE = INPUT  (current wiring)
//   Switch connects to GND:     SWITCH_ON = LOW,  SWITCH_PIN_MODE = INPUT_PULLUP
const int SWITCH_ON = HIGH;
const int SWITCH_PIN_MODE = INPUT;

#include "LED_Modes.h"               // blink(), alt(), run(), allOff(), resetPattern()


// ---------------------------------------------------------
// VARIABLES
// ---------------------------------------------------------

LiquidCrystal_I2C lcd(0x27, 16, 2);  // address 0x27 (try 0x3F if the LCD is blank), 16x2

// Mode numbers (the Java GUI uses the same numbers)
const int MODE_NONE  = 0;
const int MODE_BLINK = 1;
const int MODE_RUN   = 2;
const int MODE_ALT   = 3;

const char* names[] = {"NONE", "BLINK", "RUN", "ALT"};   // names[mode] = text for the LCD

int mode = MODE_NONE;                // current mode
unsigned long lastStepTime = 0;      // when the last 0.5 s step happened

// Switch debounce (switches "bounce" for a few ms when you flip them)
int lastSwitchMode = -1;             // what the switches asked for on the last check
int appliedSwitchMode = -1;          // the switch value we already acted on
unsigned long switchChangeTime = 0;  // when the switches last changed
const unsigned long SETTLE_MS = 50;  // the switches must stay the same this long


// ---------------------------------------------------------
// SHOW / REPORT / CHANGE THE MODE
// ---------------------------------------------------------

void showLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LED MODE:");
  lcd.setCursor(0, 1);
  lcd.print(names[mode]);
}

// Tell the GUI the current mode
void report() {
  Serial.print("MODE:");
  Serial.println(mode);
}

void setMode(int newMode) {
  if (newMode != mode) {             // only restart if the mode really changed
    mode = newMode;
    resetPattern();                  // start the pattern again (LEDs off)
    lastStepTime = 0;                // first step happens right away
    showLCD();
  }
  report();
}


// ---------------------------------------------------------
// INPUTS
// ---------------------------------------------------------

// Messages from the Java GUI
void checkSerial() {
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '1') setMode(MODE_BLINK);
    if (c == '2') setMode(MODE_RUN);
    if (c == '3') setMode(MODE_ALT);
    if (c == '?') report();
  }
}

// Read the 2 switches and return the mode they ask for
int readSwitches() {
  bool sw1On = (digitalRead(SW1) == SWITCH_ON);
  bool sw2On = (digitalRead(SW2) == SWITCH_ON);

  if (sw1On && sw2On)   return MODE_RUN;     // 11
  if (sw1On && !sw2On)  return MODE_ALT;     // 10
  if (!sw1On && sw2On)  return MODE_BLINK;   // 01
  return MODE_NONE;                          // 00
}

// Only reacts when the switches CHANGE, so the GUI can still pick a mode
// while the switches sit still (e.g. switches at 00, then press a GUI button).
void checkSwitches() {
  int wanted = readSwitches();

  // Debounce: wait until the switches stop changing
  if (wanted != lastSwitchMode) {
    lastSwitchMode = wanted;
    switchChangeTime = millis();
  }
  if (millis() - switchChangeTime < SETTLE_MS) return;

  if (wanted == appliedSwitchMode) return;   // nothing new
  appliedSwitchMode = wanted;

  setMode(wanted);
}


// ---------------------------------------------------------
// SETUP AND LOOP
// ---------------------------------------------------------

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(LED[i], OUTPUT);
  }
  pinMode(SW1, SWITCH_PIN_MODE);
  pinMode(SW2, SWITCH_PIN_MODE);

  Wire.begin(21, 22);                // LCD: SDA = 21, SCL = 22
  lcd.init();
  lcd.backlight();
  showLCD();
  report();                          // sends "MODE:0"
}

void loop() {
  checkSerial();
  checkSwitches();

  // every 0.5 s: do one step of the current mode
  if (millis() - lastStepTime >= 500) {
    lastStepTime = millis();

    if (mode == MODE_BLINK) blink();
    if (mode == MODE_RUN)   run();
    if (mode == MODE_ALT)   alt();
  }
}
