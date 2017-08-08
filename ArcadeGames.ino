/*
   Source: https://github.com/mfalkvidd/MySensors-ArcadeGames
   Author: https://github.com/mfalkvidd
   See included files for their respective authors
*/

#define ATTRACT sprintf((char *)AttractMsg, "%sTETRIS%sSCORE %u%sHIGH %u%sANY BUTTON TO START%s", BlankMsg, BlankMsg, LastScore, BlankMsg, HighScore, BlankMsg, BlankMsg);
// Enable and select radio type attached
#define MY_RF24_CHANNEL 42
#define MY_RADIO_NRF24

// Set LOW transmit power level as default, if you have an amplified NRF-module and
// power your radio separately with a good regulator you can turn up PA level.
#define MY_RF24_PA_LEVEL RF24_PA_LOW

// Enable serial gateway
#define MY_GATEWAY_SERIAL

#include <MySensors.h>

#define NUM_BUTTONS 6
bool button_state[NUM_BUTTONS];

// Based on https://github.com/AaronLiddiment/LEDSprites/blob/master/examples/Tetris/Tetris.ino
// TETRIS
// A simple Tetris game to show the use of my cLEDMatrix, cLEDText & cLEDSprite classes using the FastlED library.
// It uses 47.5k rom and 6k ram.

#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>

#include <LEDMatrix.h>
#include <LEDSprites.h>
#include <LEDText.h>
#include <FontMatrise.h>
#include "TetrisI.h"
#include "ESP8266WiFi.h"

#define BRIGHTNESS 160 // 0-255, higher values draw more power and might be more than USB can provide
#define MAX_FPS 8 // For Snake
// Change the next 6 defines to match your matrix type and size

#define LED_PIN        5
#define COLOR_ORDER    GRB
#define CHIPSET        WS2812B
#define MATRIX_WIDTH   9
#define MATRIX_HEIGHT  16
#define MATRIX_TYPE    VERTICAL_ZIGZAG_MATRIX
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)

// NOTE the '-' sign before the width, this is due to my leds matrix origin being on the right hand side
cLEDMatrix < -MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE > leds;

#define TARGET_FRAME_TIME    25  // Desired delay between updates (in milliseconds), though if too many leds it will just run as fast as it can!
#define INITIAL_DROP_FRAMES  20  // Start of game block drop delay in frames

#define A_PIN       0
#define B_PIN       1
#define UP_PIN      2
#define ROTATE_PIN  UP_PIN
#define LEFT_PIN    3
#define RIGHT_PIN   4
#define DOWN_PIN    5

#define TETRIS_SPR_WIDTH  4
#define TETRIS_SPR_HEIGHT 4
const uint8_t *TetrisSprData[] = { TetrisIData, TetrisJData, TetrisLData, TetrisOData, TetrisSData, TetrisTData, TetrisZData };
const uint8_t *TetrisSprMask[] = { TetrisIMask, TetrisJMask, TetrisLMask, TetrisOMask, TetrisSMask, TetrisTMask, TetrisZMask};
const struct CRGB TetrisColours[] = { CRGB(0, 255, 255), CRGB(0, 0, 255), CRGB(255, 125, 0), CRGB(255, 255, 0), CRGB(50, 205, 50), CRGB(255, 0, 255), CRGB(255, 0, 0) };
uint8_t next_block = random8(sizeof(TetrisSprData) / sizeof(TetrisSprData[0]));

uint8_t PlayfieldData[MATRIX_HEIGHT * ((MATRIX_WIDTH + 7) / 8) * _3BIT];
uint8_t PlayfieldMask[MATRIX_HEIGHT * ((MATRIX_WIDTH + 7) / 8) * _1BIT];
uint8_t CompletedLinesData[TETRIS_SPR_HEIGHT * ((MATRIX_WIDTH + 7) / 8) * _1BIT];
const struct CRGB CompletedLinesColour[] = { CRGB(255, 255, 255) };
cSprite Playfield, CompletedLines, CurrentBlock, NextBlockHint;
cLEDSprites Sprites(&leds);

unsigned char AttractMsg[144], GameOverMsg[88];
char BlankMsg[32];
cLEDText TetrisMsg;

uint8_t DropDelay;
boolean AttractMode, NextBlock;
int16_t TotalLines;

uint16_t PlasmaTime, PlasmaShift;
uint32_t LoopDelayMS, LastLoop;

uint8_t digitalReadWrapper(byte m_pin) {
  //return digitalRead(m_pin);
  return !button_state[m_pin];
}

void SaveHighScore(unsigned int HighScore) {
  saveState(0, HighScore & 0xFF);
  saveState(1, ((HighScore >> 8) & 0xFF));
}

unsigned int LoadHighScore() {
  // For Tetris
  unsigned int HighScore = loadState(0) + (loadState(1) << 8);
  // EEPROM is normally initialized to 0xFF
  return HighScore == 0xFFFF ? 0 : HighScore;
}

unsigned int HighScore = LoadHighScore(), LastScore;

enum class Gamemode
{
  TETRIS,
  SNAKE,
  MOODLIGHT,
  FIRE,
  LIGHT,
};

Gamemode mode = Gamemode::TETRIS;

void nextMode() {
  switch (mode) {
    case Gamemode::TETRIS:
      mode = Gamemode::SNAKE;
      break;
    case Gamemode::SNAKE:
      mode = Gamemode::MOODLIGHT;
      break;
    case Gamemode::MOODLIGHT:
      mode = Gamemode::FIRE;
      break;
    case Gamemode::FIRE:
      mode = Gamemode::LIGHT;
      break;
    case Gamemode::LIGHT:
      mode = Gamemode::TETRIS;
      break;
  }
  FastLED.clear();
  FastLED.show();
}

// Joystick class to handle input debounce along with variable delays and repeat option
class cJoyStick
{
  public:
    cJoyStick(uint8_t pin, uint16_t Debouncems, uint16_t Delayms, boolean Repeat)
    {
      m_pin = pin;
      m_DebounceMS = Debouncems;
      m_DelayMS = Delayms;
      m_Repeat = Repeat;
      m_LastMS = millis();
      m_state = digitalReadWrapper(m_pin);
    }
    boolean Read()
    {
      uint8_t state = digitalReadWrapper(m_pin);
      uint32_t ms = millis();
      if ((state != m_state) && ((ms - m_LastMS) >= m_DebounceMS))
      {
        m_state = state;
        m_LastMS = ms;
        return (true);
      }
      if ( (m_Repeat) && ((ms - m_LastMS) >= m_DelayMS) )
      {
        m_LastMS = ms;
        return (true);
      }
      return (false);
    }
    uint8_t GetState()
    {
      return (m_state);
    }
    void SetRepeat(boolean Repeat)
    {
      m_Repeat = Repeat;
    }
  protected:
    uint8_t m_pin, m_state;
    uint16_t m_DebounceMS, m_DelayMS;
    uint32_t m_LastMS;
    boolean m_Repeat;
};

cJoyStick JSRotate(ROTATE_PIN, 10, 250, false);
cJoyStick JSLeft(LEFT_PIN, 10, 250, false);
cJoyStick JSRight(RIGHT_PIN, 10, 250, false);
cJoyStick JSDown(DOWN_PIN, 10, 50, false);

#include "snake.h"
snake the_snake(3, vector2d(4, 4), vector2d(MATRIX_WIDTH, MATRIX_HEIGHT));

#include "fire.h"

void setup()
{
  // Turn off Wifi
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds[0], leds.Size());
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setCorrection(TypicalSMD5050);
  FastLED.clear(true);
  delay(50);
  FastLED.showColor(CRGB::Red);
  delay(500);
  FastLED.showColor(CRGB::Lime);
  delay(500);
  FastLED.showColor(CRGB::Blue);
  delay(500);
  FastLED.showColor(CRGB::Purple);
  delay(500);
  FastLED.show();

  setupTetris();
  setupFire();
}

void setupTetris() {
  memset(PlayfieldData, 0, sizeof(PlayfieldData));
  memset(PlayfieldMask, 0, sizeof(PlayfieldMask));
  Playfield.Setup(leds.Width(), leds.Height(), PlayfieldData, 1, _3BIT, TetrisColours, PlayfieldMask);
  Playfield.SetPositionFrameMotionOptions(0, 0, 0, 0, 0, 0, 0, 0, 0);
  Sprites.AddSprite(&Playfield);

  memset(CompletedLinesData, 0, sizeof(CompletedLinesData));
  CompletedLines.Setup(leds.Width(), TETRIS_SPR_HEIGHT, CompletedLinesData, 1, _1BIT, CompletedLinesColour, CompletedLinesData);
  CompletedLines.SetPositionFrameMotionOptions(0, 0, 0, 0, 0, 0, 0, 0, 0);

  TetrisMsg.SetFont(MatriseFontData);
  sprintf((char *)BlankMsg, "%*s", _min(((leds.Height() + TetrisMsg.FontHeight()) / (TetrisMsg.FontHeight() + 1)), (int)sizeof(BlankMsg) - 1), "");
  ATTRACT;
  TetrisMsg.Init(&leds, TetrisMsg.FontWidth() + 1, leds.Height(), (leds.Width() - TetrisMsg.FontWidth()) / 2, 0);
  TetrisMsg.SetBackgroundMode(BACKGND_LEAVE);
  TetrisMsg.SetScrollDirection(SCROLL_UP);
  TetrisMsg.SetTextDirection(CHAR_UP);
  TetrisMsg.SetFrameRate(1);
  TetrisMsg.SetOptionsChangeMode(INSTANT_OPTIONS_MODE);
  TetrisMsg.SetText(AttractMsg, strlen((const char *)AttractMsg));
  AttractMode = true;
  LoopDelayMS = TARGET_FRAME_TIME;
  LastLoop = millis() - LoopDelayMS;
  PlasmaShift = (random8(0, 5) * 32) + 64;
  PlasmaTime = 0;
}

void loop() {
  switch (mode) {
    case Gamemode::TETRIS :
      loopTetris();
      break;
    case Gamemode::SNAKE :
      loopSnake();
      break;
    case Gamemode::MOODLIGHT :
      loopMoodlight();
      break;
    case Gamemode::FIRE :
      loopFire();
      break;
    case Gamemode::LIGHT :
      looplight();
      break;
  }
}

#define MIN_UPDATE_DELAY 150
#define HUE_ADJUSTMENT_FACTOR 16
#define DIM_ADJUSTMENT_FACTOR 20
void loopMoodlight() {
  static byte currentHue = HUE_ORANGE;
  static int dimLevel = 0;
  static unsigned long lastUpdate = millis();
  if ((millis() - lastUpdate > MIN_UPDATE_DELAY)) {
    // Don't check buttons too frequently
    if (button_state[LEFT_PIN]) {
      // Decrease hue
      currentHue = currentHue - HUE_ADJUSTMENT_FACTOR;
      Serial.print("currentHue: "); Serial.println(currentHue);
    }
    if (button_state[UP_PIN]) {
      // Decrease dimming
      dimLevel = constrain(dimLevel - DIM_ADJUSTMENT_FACTOR, 0, 255);
      Serial.print("dimLevel: "); Serial.println(dimLevel);
    }
    if (button_state[DOWN_PIN]) {
      // Increase dimming
      dimLevel = constrain(dimLevel + DIM_ADJUSTMENT_FACTOR, 0, 255);
      Serial.print("dimLevel: "); Serial.println(dimLevel);
    }
    if (button_state[RIGHT_PIN]) {
      // Increase hue
      currentHue = currentHue + HUE_ADJUSTMENT_FACTOR;
      Serial.print("currentHue: "); Serial.println(currentHue);
    }
    lastUpdate = millis();
  }
  CRGB moodColorRGB;
  hsv2rgb_rainbow(CHSV(currentHue, 255, 255), moodColorRGB);
  moodColorRGB.subtractFromRGB(dimLevel);
  FastLED.showColor(moodColorRGB);
}

void looplight() {
  static unsigned long currentWhite = Candle;
  static int dimLevel = 0;
  CRGB lightColorRGB;
  static unsigned long lastUpdate = millis();
  if ((millis() - lastUpdate > MIN_UPDATE_DELAY)) {
    // Don't check buttons too frequently
    if (button_state[LEFT_PIN]) {
      // make light warmer
      switch (currentWhite) {
        case ClearBlueSky:
          currentWhite = Candle; // 1900 K
          break;
        case Candle:
          currentWhite = Tungsten100W; // 2850 K
          break;
        case Tungsten100W:
          currentWhite = Halogen; // 3200 K
          break;
        case Halogen:
          currentWhite = DirectSunlight; // 6000 K
          break;
        case DirectSunlight:
          currentWhite = ClearBlueSky; // 20000 K
          break;
      }
    }
    if (button_state[UP_PIN]) {
      // Decrease dimming
      dimLevel = constrain(dimLevel - DIM_ADJUSTMENT_FACTOR, 0, 255);
    }
    if (button_state[DOWN_PIN]) {
      // Increase dimming
      dimLevel = constrain(dimLevel + DIM_ADJUSTMENT_FACTOR, 0, 255);
    }
    if (button_state[RIGHT_PIN]) {
      // make light cooler
      switch (currentWhite) {
        case ClearBlueSky:
          currentWhite = DirectSunlight;
          break;
        case DirectSunlight:
          currentWhite = Halogen;
          break;
        case Halogen:
          currentWhite = Tungsten100W;
          break;
        case Tungsten100W:
          currentWhite = Candle;
          break;
        case Candle:
          currentWhite = ClearBlueSky;
          break;
      }
    }
    lastUpdate = millis();
  }
  lightColorRGB = currentWhite;
  lightColorRGB.subtractFromRGB(dimLevel);
  FastLED.showColor(lightColorRGB);
}

void loopTetris()
{
  if (abs(millis() - LastLoop) < LoopDelayMS)
  { // If it is not yet time to update, just let FastLED dither and return
    FastLED.show();
    return;
  }

  LastLoop = millis();
  FastLED.clear();

  // Fill background with dim plasma
#define PLASMA_X_FACTOR  24
#define PLASMA_Y_FACTOR  24
  for (int16_t x = 0; x < MATRIX_WIDTH; x++)
  {
    for (int16_t y = 0; y < MATRIX_HEIGHT; y++)
    {
      int16_t r = sin16(PlasmaTime) / 256;
      int16_t h = sin16(x * r * PLASMA_X_FACTOR + PlasmaTime) + cos16(y * (-r) * PLASMA_Y_FACTOR + PlasmaTime) + sin16(y * x * (cos16(-PlasmaTime) / 256) / 2);
      leds(x, y) = CHSV((uint8_t)((h / 256) + 128), 255, 64);
    }
  }
  uint16_t OldPlasmaTime = PlasmaTime;
  PlasmaTime += PlasmaShift;
  if (OldPlasmaTime > PlasmaTime)
    PlasmaShift = (random8(0, 5) * 32) + 64;

  if (AttractMode)
  {
    if ( ((JSRotate.Read()) && (JSRotate.GetState() == LOW)) || ((JSLeft.Read()) && (JSLeft.GetState() == LOW))
         || ((JSRight.Read()) && (JSRight.GetState() == LOW)) || ((JSDown.Read()) && (JSDown.GetState() == LOW)) )
    {
      JSRotate.SetRepeat(true);
      JSLeft.SetRepeat(true);
      JSRight.SetRepeat(true);
      JSDown.SetRepeat(true);
      AttractMode = false;
      memset(PlayfieldData, 0, sizeof(PlayfieldData));
      memset(PlayfieldMask, 0, sizeof(PlayfieldMask));
      Sprites.RemoveSprite(&CurrentBlock);
      LastScore = 0;
      TotalLines = 0;
      DropDelay = INITIAL_DROP_FRAMES;
      CurrentBlock.SetXChange(-1);
      NextBlock = true;
    }
  }
  else
  {
    if (Sprites.IsSprite(&CompletedLines))  // We have highlighted complete lines, delay for visual effect
    {
      if (CompletedLines.GetXCounter() > 0)
        CompletedLines.SetXCounter(CompletedLines.GetXCounter() - 1);
      else
      {
        Sprites.RemoveSprite(&CompletedLines);
        // Remove completed lines from playfield sprite
        uint8_t *Data = PlayfieldData;
        uint8_t *Mask = PlayfieldMask;
        uint16_t Mbpl = (MATRIX_WIDTH + 7) / 8;
        uint16_t Dbpl = Mbpl * _3BIT;
        int16_t k;
        for (int16_t i = (MATRIX_HEIGHT - 1) * Dbpl, j = (MATRIX_HEIGHT - 1) * Mbpl; i >= 0; i -= Dbpl, j -= Mbpl)
        {
          for (k = 0; k < MATRIX_WIDTH; k += 8)
          {
            if ((uint8_t)(0xff00 >> _min(MATRIX_WIDTH - k, 8)) != Mask[j + (k / 8)])
              break;
          }
          if (k >= MATRIX_WIDTH)
          {
            memmove(&Data[Dbpl], &Data[0], i);
            memset(&Data[0], 0, Dbpl);
            memmove(&Mask[Mbpl], &Mask[0], j);
            memset(&Mask[0], 0, Mbpl);
            i += Dbpl;
            j += Mbpl;
          }
        }
      }
    }
    else
    {
      if (CurrentBlock.GetXChange() >= 0) // We have a current block
      {
        // Check for user input
        if ( (JSRotate.Read()) && (JSRotate.GetState() == LOW) )
        {
          if ((CurrentBlock.GetCurrentFrame() % 2) == 1)
          {
            if (CurrentBlock.GetXChange() == 0)
              CurrentBlock.m_X = _min(CurrentBlock.m_X, MATRIX_WIDTH - TETRIS_SPR_WIDTH);
            else if ((CurrentBlock.GetXChange() != 3) && (CurrentBlock.GetFlags() & SPRITE_EDGE_X_MAX))
              --CurrentBlock.m_X;
          }
          CurrentBlock.IncreaseFrame();
          Sprites.DetectCollisions(&CurrentBlock);
          if (CurrentBlock.GetFlags() & SPRITE_COLLISION)
            CurrentBlock.DecreaseFrame();
        }
        if ( (JSLeft.Read()) && (JSLeft.GetState() == LOW) && (! (CurrentBlock.GetFlags() & SPRITE_EDGE_X_MIN)) )
        {
          CurrentBlock.m_X--;
          Sprites.DetectCollisions(&CurrentBlock);
          if (CurrentBlock.GetFlags() & SPRITE_COLLISION)
            CurrentBlock.m_X++;
        }
        else if ( (JSRight.Read()) && (JSRight.GetState() == LOW) && (! (CurrentBlock.GetFlags() & SPRITE_EDGE_X_MAX)) )
        {
          CurrentBlock.m_X++;
          Sprites.DetectCollisions(&CurrentBlock);
          if (CurrentBlock.GetFlags() & SPRITE_COLLISION)
            CurrentBlock.m_X--;
        }
        if ( (JSDown.Read()) && (JSDown.GetState() == LOW) )
          CurrentBlock.SetYCounter(1);
        // Do block checks for bottom or collision
        if (CurrentBlock.GetYCounter() <= 1)
        {
          if (CurrentBlock.GetFlags() & SPRITE_EDGE_Y_MIN)
            NextBlock = true;
          else
          {
            --CurrentBlock.m_Y;
            Sprites.DetectCollisions(&CurrentBlock);
            ++CurrentBlock.m_Y;
            if (CurrentBlock.GetFlags() & SPRITE_COLLISION)
            {
              // Block has collided check for game over
              int16_t MaxY = MATRIX_HEIGHT - 2;
              if ((CurrentBlock.GetCurrentFrame() % 2) == 1)
              {
                if (CurrentBlock.GetXChange() == 0)
                  MaxY -= 2;
                else if (CurrentBlock.GetXChange() != 3)
                  MaxY -= 1;
              }
              else if (CurrentBlock.GetXChange() == 0)
                ++MaxY;
              if (CurrentBlock.m_Y < MaxY)
                NextBlock = true;
              else
              {
                // Game over
                CurrentBlock.SetYCounter(2);  // Stop last block moving down!
                AttractMode = true;
                JSRotate.SetRepeat(false);
                JSLeft.SetRepeat(false);
                JSRight.SetRepeat(false);
                JSDown.SetRepeat(false);
                if (LastScore > HighScore)
                {
                  HighScore = LastScore;
                  SaveHighScore(HighScore);
                  sprintf((char *)GameOverMsg, "%sGAME OVER%sNEW HIGH SCORE %u%s", BlankMsg, BlankMsg, LastScore, BlankMsg);
                }
                else
                {
                  sprintf((char *)GameOverMsg, "%sGAME OVER%sSCORE %u%s", BlankMsg, BlankMsg, LastScore, BlankMsg);
                }
                ATTRACT;
                TetrisMsg.SetText(GameOverMsg, strlen((char *)GameOverMsg));
                TetrisMsg.SetBackgroundMode(BACKGND_DIMMING, 0x40);
              }
            }
          }
        }
      }
      if (NextBlock)  // Start new block
      {
        if (CurrentBlock.GetXChange() >= 0) // We have a current block so add to playfield before creating new block
        {
          Playfield.Combine(CurrentBlock.m_X, CurrentBlock.m_Y, &CurrentBlock);
          Sprites.RemoveSprite(&CurrentBlock);
          // Make completed lines highlight sprite & score
          memset(CompletedLinesData, 0, sizeof(CompletedLinesData));
          CompletedLines.m_Y = -1;
          uint8_t *Mask = PlayfieldMask;
          uint16_t Mbpl = (MATRIX_WIDTH + 7) / 8;
          int16_t j, numlines = 0;
          for (int16_t i = (MATRIX_HEIGHT - 1) * Mbpl, y = 0; i >= 0; i -= Mbpl, ++y)
          {
            for (j = 0; j < MATRIX_WIDTH; j += 8)
            {
              if ((uint8_t)(0xff00 >> _min(MATRIX_WIDTH - j, 8)) != Mask[i + (j / 8)])
                break;
            }
            if (j >= MATRIX_WIDTH)
            {
              if (CompletedLines.m_Y == -1)
                CompletedLines.m_Y = y;
              memset(&CompletedLinesData[((TETRIS_SPR_HEIGHT - 1) - (y - CompletedLines.m_Y)) * Mbpl], 0xff, Mbpl);
              numlines++;
            }
          }
          if (numlines > 0)
          {
            CompletedLines.SetXCounter(15);  // Set delay for highlight display to 15 loops
            Sprites.AddSprite(&CompletedLines);
          }
          LastScore += 1;
          if (numlines == 1)
            LastScore += 4;
          else if (numlines == 2)
            LastScore += 12;
          else if (numlines == 3)
            LastScore += 20;
          else if (numlines == 4)
            LastScore += 40;
          TotalLines += numlines;
          DropDelay = _max(1, INITIAL_DROP_FRAMES - (TotalLines / 5));
        }
        // Start new block
        uint8_t j = next_block;
        next_block = random8(sizeof(TetrisSprData) / sizeof(TetrisSprData[0]));

        CurrentBlock.Setup(TETRIS_SPR_WIDTH, TETRIS_SPR_WIDTH, TetrisSprData[j], 4, _3BIT, TetrisColours, TetrisSprMask[j]);
        CurrentBlock.SetPositionFrameMotionOptions((MATRIX_WIDTH / 2) - 1, MATRIX_HEIGHT, 0, 0, 0, 0, -1, DropDelay, SPRITE_DETECT_COLLISION | SPRITE_DETECT_EDGE);
        CurrentBlock.SetXChange(j);
        Sprites.AddSprite(&CurrentBlock);

        Sprites.RemoveSprite(&NextBlockHint);
        NextBlockHint.Setup(TETRIS_SPR_WIDTH, TETRIS_SPR_WIDTH, TetrisSprData[next_block], 4, _3BIT, TetrisColours, TetrisSprMask[next_block]);
        NextBlockHint.SetPositionFrameMotionOptions(MATRIX_WIDTH - 1, MATRIX_HEIGHT - 3, 0, 0, 0, 0, 0, 0, 0);
        Sprites.AddSprite(&NextBlockHint);
        NextBlock = false;
      }
      Sprites.UpdateSprites();
    }
  }
  Sprites.RenderSprites();

  if (AttractMode)
  {
    if (TetrisMsg.UpdateText() == -1)
    {
      TetrisMsg.SetText(AttractMsg, strlen((char *)AttractMsg));
      TetrisMsg.SetBackgroundMode(BACKGND_LEAVE);
      Sprites.RemoveSprite(&CurrentBlock);
      Sprites.RemoveSprite(&NextBlockHint);
      memset(PlayfieldData, 0, sizeof(PlayfieldData));
      memset(PlayfieldMask, 0, sizeof(PlayfieldMask));
    }
  }
  FastLED.show();
}

void receive(const MyMessage &message) {
  button_state[message.sensor] = message.getBool();
  if (button_state[A_PIN] && button_state[B_PIN]) {
    // A+B buttons are pressed at the same time. Switch mode.
    nextMode();
  }
}

void loopSnake() {
  uint32_t start_frame = millis();

  if (button_state[LEFT_PIN]) {
    the_snake.changeDirection(Left);
  }
  if (button_state[UP_PIN]) {
    the_snake.changeDirection(Up);
  }
  if (button_state[DOWN_PIN]) {
    the_snake.changeDirection(Down);
  }
  if (button_state[RIGHT_PIN]) {
    the_snake.changeDirection(Right);
  }

  FastLED.clear();

  leds(the_snake.getFood().x, the_snake.getFood().y) = CRGB(255, 0, 0); //= CHSV((uint8_t)((h / 256) + 128), 255, 64);

  snake_part *s = the_snake.getHead();
  leds(s->pos.x, s->pos.y) = CRGB(255, 255, 0);
  s = s->next;

  while (s != nullptr) {
    leds(s->pos.x, s->pos.y) = CRGB(0, 255, 0);
    s = s->next;
  }

  the_snake.update();

  FastLED.show();
  wait((1000 / MAX_FPS) - (millis() - start_frame));
}

