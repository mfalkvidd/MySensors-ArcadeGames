/*
   LICENSE: GPLv2
   Source: https://github.com/darrenpmeyer/Arduino-FireBoard/
   Author https://github.com/darrenpmeyer/
*/

#include <FastLED.h>
CRGBPalette16 gPal;

void setupFire() {
  /* Set a black-body radiation palette
     This comes from FastLED */
  gPal = HeatColors_p;
}

/* Refresh rate. Higher makes for flickerier
   Recommend small values for small displays */
#define FPS 17
#define FPS_DELAY 1000/FPS

/* Rate of cooling. Play with to change fire from
   roaring (smaller values) to weak (larger values) */
int cooling = 40;

/* How hot is "hot"? Increase for brighter fire */
int hotness = 100;

void Fireplace () {
  static unsigned int spark[MATRIX_WIDTH]; // base heat
  CRGB stack[MATRIX_WIDTH][MATRIX_HEIGHT];        // stacks that are cooler

  // 1. Generate sparks to re-heat
  for (int i = 0; i < MATRIX_WIDTH; i++) {
    if (spark[i] < hotness) {
      int base = hotness * 2;
      spark[i] = random16(base, hotness * MATRIX_HEIGHT);
    }
  }

  // 2. Cool all the sparks
  for (int i = 0; i < MATRIX_WIDTH; i++) {
    spark[i] = qsub8(spark[i], random8(0, cooling));
  }

  // 3. Build the stack
  /*    This works on the idea that pixels are "cooler"
        as they get further from the spark at the bottom */
  for (int i = 0; i < MATRIX_WIDTH; i++) {
    unsigned int heat = constrain(spark[i], hotness / 2, hotness * MATRIX_HEIGHT);
    for (int j = MATRIX_HEIGHT - 1; j >= 0; j--) {
      /* Calculate the color on the palette from how hot this
         pixel is */
      byte index = constrain(heat, 0, hotness);
      stack[i][j] = ColorFromPalette(gPal, index);

      /* The next higher pixel will be "cooler", so calculate
         the drop */
      unsigned int drop = random8(0, hotness);
      if (drop > heat) heat = 0; // avoid wrap-arounds from going "negative"
      else heat -= drop;

      heat = constrain(heat, 0, hotness * MATRIX_HEIGHT);
    }
  }

  // 4. map stacks to led array
  for (int i = 0; i < MATRIX_WIDTH; i++) {
    for (int j = 0; j < MATRIX_HEIGHT; j++) {
      leds(i, MATRIX_HEIGHT - 1 - j) = stack[i][j];
    }
  }
}

#define MAX_HOTNESS 180
#define MIN_HOTNESS 40
#define ADJUSTMENT_FACTOR 20
#define MIN_UPDATE_DELAY 150
void checkButtonsFire() {
  static unsigned long lastUpdate = millis();
  if (!(millis() - lastUpdate > MIN_UPDATE_DELAY)) return; // Just return if it is not yet time to update
  if (button_state[LEFT_PIN]) {
    hotness = constrain(hotness - ADJUSTMENT_FACTOR, MIN_HOTNESS, MAX_HOTNESS);
  }
  if (button_state[UP_PIN]) {
    cooling = constrain(cooling - ADJUSTMENT_FACTOR, 0, 255);
  }
  if (button_state[DOWN_PIN]) {
    cooling = constrain(cooling + ADJUSTMENT_FACTOR, 0, 255);
  }
  if (button_state[RIGHT_PIN]) {
    hotness = constrain(hotness + ADJUSTMENT_FACTOR, MIN_HOTNESS, MAX_HOTNESS);
  }
  lastUpdate = millis();
}

void loopFire() {
  random16_add_entropy(rand()); // We chew a lot of entropy
  checkButtonsFire();

  Fireplace();

  FastLED.show();
  wait(FPS_DELAY);
}

