/*===========================================================
  LED_Modes.h  -  the three LED modes

  The main sketch must define LED[], NUM_LEDS and HALF
  BEFORE it includes this file.

  Each mode does ONE step. The main sketch calls it every 0.5 s.
-----------------------------------------------------------  */

bool flip = false;     // BLINK and ALT: switches between true and false each step
int pos = 0;           // RUN: which LED is on right now
int dir = 1;           // RUN: 1 = going forward, -1 = going backward


// Turn LEDs number "from" up to (but not including) "to" ON or OFF.
// value is HIGH (on) or LOW (off).
void setLEDs(int from, int to, int value) {
  for (int i = from; i < to; i++) {
    digitalWrite(LED[i], value);
  }
}

void allOff() {
  setLEDs(0, NUM_LEDS, LOW);
}

// Go back to the start of the pattern (used when the mode changes)
void resetPattern() {
  flip = false;
  pos = 0;
  dir = 1;
  allOff();
}

// BLINK: all ON, then all OFF
void blink() {
  if (flip == false) {
    setLEDs(0, NUM_LEDS, HIGH);
  } else {
    setLEDs(0, NUM_LEDS, LOW);
  }
  flip = !flip;                        // ! means "the opposite"
}

// ALT: first half ON, then second half ON
void alt() {
  if (flip == false) {
    setLEDs(0, HALF, HIGH);            // first half ON
    setLEDs(HALF, NUM_LEDS, LOW);      // second half OFF
  } else {
    setLEDs(0, HALF, LOW);             // first half OFF
    setLEDs(HALF, NUM_LEDS, HIGH);     // second half ON
  }
  flip = !flip;
}

// RUN: one LED at a time, from the first LED to the last, then back again
void run() {
  allOff();
  digitalWrite(LED[pos], HIGH);        // light only the LED at "pos"

  if (pos == NUM_LEDS - 1) dir = -1;   // at the last LED  -> turn around
  if (pos == 0)            dir = 1;    // at the first LED -> turn around

  pos = pos + dir;                     // move to the next LED
}
