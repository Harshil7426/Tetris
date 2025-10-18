
// --- Core Libraries
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

// =========================================================================
// ==                      CONFIGURATION (ADJUST PINS HERE)               ==
// =========================================================================

#define TFT_CS 10
#define TFT_RST 8
#define TFT_DC 9
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

#define OLED_SCREEN_WIDTH 128
#define OLED_SCREEN_HEIGHT 64
#define OLED_RESET 0
Adafruit_SSD1306 scoreDisplay(OLED_SCREEN_WIDTH, OLED_SCREEN_HEIGHT, &Wire, OLED_RESET);

#define ROWS 3
#define COLS 3
byte rowPins[ROWS] = { 2, 3, 4 };
byte colPins[COLS] = { 5, 6, 7 };
char keys[ROWS][COLS] = {
  { ' ', 'U', ' ' },
  { 'L', 'S', 'R' },
  { ' ', 'D', ' ' }
};

#define SPEAKER_PIN A1

// =========================================================================
// ==                         SOUND DEFINITIONS                           ==
// =========================================================================
#define NOTE_A3  220
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_G5  784
#define NOTE_A5  880
#define NOTE_C6  1047

int startScreenMelody[] = { NOTE_E4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_D4, 0 };
int startScreenDurations[] = { 8, 8, 4, 4, 8, 8, 4, 4, 8, 8, 4, 4, 8, 8, 4, 4 };

// NEW: Melody for the level select screen
int levelSelectMelody[] = { NOTE_C4, NOTE_E4, NOTE_G4, NOTE_E4, NOTE_C4, 0 };
int levelSelectDurations[] = { 4, 4, 4, 4, 2, 2 };

int melody[] = { NOTE_E5, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_D5, NOTE_C5, NOTE_B4, NOTE_A4, NOTE_A4, NOTE_C5, NOTE_E5, NOTE_D5, NOTE_C5, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_C5, NOTE_A4, NOTE_A4, 0 };
int noteDurations[] = { 4, 8, 8, 4, 8, 8, 4, 4, 4, 8, 8, 4, 8, 8, 4, 8, 8, 4, 4, 4, 4, 4 };
int currentNote = 0;
unsigned long lastNoteTime = 0;
bool musicPlaying = false;


// =========================================================================
// ==                         GAME CONSTANTS & VARIABLES                  ==
// =========================================================================
#define BOARD_WIDTH 16
#define BOARD_HEIGHT 20
#define BLOCK_SIZE 12
#define BOARD_X_OFFSET ((250 - (BOARD_WIDTH * BLOCK_SIZE)) / 2)
#define BOARD_Y_OFFSET ((200 - (BOARD_HEIGHT * BLOCK_SIZE)) / 2)

byte board[BOARD_HEIGHT][BOARD_WIDTH];
int score = 0;
int level = 1;
int linesCleared = 0;
unsigned long fallSpeed = 1000;
unsigned long lastFallTime = 0;
bool gameOver = false;

#define EEPROM_ADDR 0
int highScore = 0;

const byte PIECES[7][4][4][4] = {
  { { { 0, 0, 0, 0 }, { 0, 1, 1, 0 }, { 0, 1, 1, 0 }, { 0, 0, 0, 0 } }, { { 0, 0, 0, 0 }, { 0, 1, 1, 0 }, { 0, 1, 1, 0 }, { 0, 0, 0, 0 } }, { { 0, 0, 0, 0 }, { 0, 1, 1, 0 }, { 0, 1, 1, 0 }, { 0, 0, 0, 0 } }, { { 0, 0, 0, 0 }, { 0, 1, 1, 0 }, { 0, 1, 1, 0 }, { 0, 0, 0, 0 } } },
  { { { 0, 0, 0, 0 }, { 1, 1, 1, 1 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { { 0, 1, 0, 0 }, { 0, 1, 0, 0 }, { 0, 1, 0, 0 }, { 0, 1, 0, 0 } }, { { 0, 0, 0, 0 }, { 1, 1, 1, 1 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { { 0, 1, 0, 0 }, { 0, 1, 0, 0 }, { 0, 1, 0, 0 }, { 0, 1, 0, 0 } } },
  { { { 0, 0, 0, 0 }, { 0, 1, 1, 0 }, { 1, 1, 0, 0 }, { 0, 0, 0, 0 } }, { { 0, 1, 0, 0 }, { 0, 1, 1, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 0 } }, { { 0, 0, 0, 0 }, { 0, 1, 1, 0 }, { 1, 1, 0, 0 }, { 0, 0, 0, 0 } }, { { 0, 1, 0, 0 }, { 0, 1, 1, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 0 } } },
  { { { 0, 0, 0, 0 }, { 1, 1, 0, 0 }, { 0, 1, 1, 0 }, { 0, 0, 0, 0 } }, { { 0, 0, 1, 0 }, { 0, 1, 1, 0 }, { 0, 1, 0, 0 }, { 0, 0, 0, 0 } }, { { 0, 0, 0, 0 }, { 1, 1, 0, 0 }, { 0, 1, 1, 0 }, { 0, 0, 0, 0 } }, { { 0, 0, 1, 0 }, { 0, 1, 1, 0 }, { 0, 1, 0, 0 }, { 0, 0, 0, 0 } } },
  { { { 0, 0, 0, 0 }, { 1, 1, 1, 0 }, { 0, 1, 0, 0 }, { 0, 0, 0, 0 } }, { { 0, 1, 0, 0 }, { 1, 1, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 0, 0 } }, { { 0, 0, 0, 0 }, { 0, 1, 0, 0 }, { 1, 1, 1, 0 }, { 0, 0, 0, 0 } }, { { 0, 1, 0, 0 }, { 0, 1, 1, 0 }, { 0, 1, 0, 0 }, { 0, 0, 0, 0 } } },
  { { { 0, 0, 0, 0 }, { 1, 1, 1, 0 }, { 1, 0, 0, 0 }, { 0, 0, 0, 0 } }, { { 1, 1, 0, 0 }, { 0, 1, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 0, 0 } }, { { 0, 0, 1, 0 }, { 1, 1, 1, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { { 0, 1, 0, 0 }, { 0, 1, 0, 0 }, { 0, 1, 1, 0 }, { 0, 0, 0, 0 } } },
  { { { 0, 0, 0, 0 }, { 1, 1, 1, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 0 } }, { { 0, 1, 0, 0 }, { 0, 1, 0, 0 }, { 1, 1, 0, 0 }, { 0, 0, 0, 0 } }, { { 1, 0, 0, 0 }, { 1, 1, 1, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { { 0, 1, 1, 0 }, { 0, 1, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 0, 0 } } }
};
const uint16_t COLORS[] = { 0, ILI9341_CYAN, ILI9341_BLUE, ILI9341_ORANGE, ILI9341_YELLOW, ILI9341_GREEN, ILI9341_PURPLE, ILI9341_RED };
int currentPiece, nextPiece;
int currentRotation, currentX, currentY;
#define NUM_FALLING_PIECES 3
#define ANIMATION_BLOCK_SIZE 6
struct FallingPiece { int type; float x, y; int rotation; float speed; uint16_t color; };
FallingPiece fallingPieces[NUM_FALLING_PIECES];

// =========================================================================
// ==                          SOUND FUNCTIONS                            ==
// =========================================================================
void playMoveSound() { tone(SPEAKER_PIN, 2000, 20); }
void playGameStartSound() {
  tone(SPEAKER_PIN, NOTE_C5, 100); delay(100);
  tone(SPEAKER_PIN, NOTE_E5, 100); delay(100);
  tone(SPEAKER_PIN, NOTE_G5, 100); delay(100);
  tone(SPEAKER_PIN, NOTE_C6, 150); delay(150);
}
void playLineClearSound() {
  tone(SPEAKER_PIN, NOTE_A5, 100); delay(100);
  tone(SPEAKER_PIN, NOTE_C6, 150); delay(150);
}
void playGameOverSound() {
  tone(SPEAKER_PIN, NOTE_G4, 250); delay(275);
  tone(SPEAKER_PIN, NOTE_D4, 250); delay(275);
  tone(SPEAKER_PIN, NOTE_C4, 400); delay(400);
}
void updateGameMusic() {
  if (!musicPlaying) return;
  int noteDuration = 1000 / noteDurations[currentNote];
  if (millis() - lastNoteTime > noteDuration) {
    lastNoteTime = millis();
    int note = melody[currentNote];
    if (note > 0) {
      tone(SPEAKER_PIN, note, noteDuration * 0.9);
    }
    currentNote++;
    if (currentNote >= (sizeof(melody)/sizeof(int))) {
      currentNote = 0;
    }
  }
}

// =========================================================================
// ==                       DRAWING & ANIMATION                           ==
// =========================================================================
void drawGradientRegion(int16_t x, int16_t y, int16_t w, int16_t h) {
  float r_start = 0, g_start = 0, b_start = 80;
  float r_end = 0, g_end = 0, b_end = 0;
  int16_t screen_h = tft.height();
  for (int16_t i = 0; i < h; i++) {
    int16_t globalY = y + i;
    if (globalY < 0 || globalY >= screen_h) {
      continue;
    }
    uint8_t b = map(globalY, 0, screen_h - 1, b_start, b_end);
    tft.drawFastHLine(x, globalY, w, tft.color565(0, 0, b));
  }
}
void drawAnimatedTetromino(int x, int y, int type, int rotation, uint16_t color) {
  for (int i = 0; i < 4; i++) { for (int j = 0; j < 4; j++) { if (PIECES[type][rotation][i][j]) { tft.fillRect(x + j * ANIMATION_BLOCK_SIZE, y + i * ANIMATION_BLOCK_SIZE, ANIMATION_BLOCK_SIZE, ANIMATION_BLOCK_SIZE, color); } } }
}
void resetPiece(FallingPiece &piece) {
    int spawnAreaWidth = BOARD_X_OFFSET - (4 * ANIMATION_BLOCK_SIZE);
    piece.x = random(0, spawnAreaWidth); piece.y = -(4 * ANIMATION_BLOCK_SIZE); piece.type = random(0, 7); piece.rotation = random(0, 4); piece.speed = random(40, 100) / 100.0f; piece.color = COLORS[piece.type + 1];
}

// =========================================================================
// ==                              SETUP                                  ==
// =========================================================================
void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(A0));
  tft.begin();
  tft.setRotation(1);
  if (!scoreDisplay.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  scoreDisplay.clearDisplay();
  scoreDisplay.display();
  for (int i = 0; i < ROWS; i++) { pinMode(rowPins[i], OUTPUT); digitalWrite(rowPins[i], HIGH); }
  for (int i = 0; i < COLS; i++) { pinMode(colPins[i], INPUT_PULLUP); }
  
  EEPROM.get(EEPROM_ADDR, highScore);

  // MODIFIED: New game flow
  showStartScreen();
  showDifficultyScreen();
  showControlsScreen(); // NEW
  startGame();
}

// =========================================================================
// ==              SHOW START SCREEN (SMOOTH 60 FPS)                      ==
// =========================================================================
void showStartScreen() {
  const int FPS = 60;
  const int FRAME_DELAY = 1000 / FPS;
  unsigned long lastFrameTime = 0;
  const int16_t TITLE_SIZE = 4;
  const int16_t TITLE_WIDTH = 6 * 6 * TITLE_SIZE;
  const int16_t TITLE_X = (tft.width() - TITLE_WIDTH) / 2;
  const int16_t TITLE_Y = 50;
  const int16_t TITLE_H = 8 * TITLE_SIZE;
  const int16_t PROMPT_SIZE = 1;
  const int16_t PROMPT_WIDTH_1 = 16 * 6 * PROMPT_SIZE;
  const int16_t PROMPT_X_1 = (tft.width() - PROMPT_WIDTH_1) / 2;
  const int16_t PROMPT_Y_1 = 160;
  const int16_t PROMPT_WIDTH_2 = 8 * 6 * PROMPT_SIZE;
  const int16_t PROMPT_X_2 = (tft.width() - PROMPT_WIDTH_2) / 2;
  const int16_t PROMPT_Y_2 = 180;
  drawGradientRegion(0, 0, tft.width(), tft.height());
  tft.setTextSize(TITLE_SIZE);
  uint16_t colors[] = {ILI9341_CYAN, ILI9341_YELLOW, ILI9341_PURPLE, ILI9341_GREEN, ILI9341_ORANGE, ILI9341_RED};
  char title[] = "TETRIS";
  for (int i = 0; i < 6; i++) {
    tft.setCursor(TITLE_X + (i * 6 * TITLE_SIZE), TITLE_Y);
    tft.setTextColor(colors[i]);
    tft.print(title[i]);
  }
  for (int i = 0; i < NUM_FALLING_PIECES; i++) {
    resetPiece(fallingPieces[i]);
    fallingPieces[i].y = random(-tft.height(), 0);
  }
  long lastBlinkTime = 0;
  bool promptVisible = true;
  bool titleNeedsRedraw = false;
  unsigned long lastStartMusicNoteTime = 0;
  int startMusicNoteIndex = 0;
  tone(SPEAKER_PIN, NOTE_C4, 150); delay(160);
  tone(SPEAKER_PIN, NOTE_E4, 150); delay(160);
  tone(SPEAKER_PIN, NOTE_G4, 150); delay(160);
  tone(SPEAKER_PIN, NOTE_C5, 300); delay(300);
  while (true) {
    if (millis() - lastFrameTime < FRAME_DELAY) { if (getKey() == 'S') { break; } continue; }
    lastFrameTime = millis();
    unsigned long currentMillis = millis();
    int duration = 1000 / startScreenDurations[startMusicNoteIndex];
    if (currentMillis - lastStartMusicNoteTime > duration) {
      lastStartMusicNoteTime = currentMillis;
      int note = startScreenMelody[startMusicNoteIndex];
      if (note > 0) { tone(SPEAKER_PIN, note, duration * 0.9); }
      startMusicNoteIndex++;
      if (startMusicNoteIndex >= (sizeof(startScreenMelody)/sizeof(int))) { startMusicNoteIndex = 0; }
    }
    if (getKey() == 'S') { noTone(SPEAKER_PIN); break; }
    for (int i = 0; i < NUM_FALLING_PIECES; i++) {
      drawGradientRegion(fallingPieces[i].x, fallingPieces[i].y, 4 * ANIMATION_BLOCK_SIZE, 4 * ANIMATION_BLOCK_SIZE);
      if (!titleNeedsRedraw) {
          if (fallingPieces[i].x < TITLE_X + TITLE_WIDTH &&
              fallingPieces[i].x + (4*ANIMATION_BLOCK_SIZE) > TITLE_X &&
              fallingPieces[i].y < TITLE_Y + TITLE_H &&
              fallingPieces[i].y + (4*ANIMATION_BLOCK_SIZE) > TITLE_Y) {
              titleNeedsRedraw = true;
          }
      }
      fallingPieces[i].y += fallingPieces[i].speed;
    }
    if (titleNeedsRedraw) {
      tft.setTextSize(TITLE_SIZE);
      for (int i = 0; i < 6; i++) {
        tft.setCursor(TITLE_X + (i * 6 * TITLE_SIZE), TITLE_Y); tft.setTextColor(colors[i]); tft.print(title[i]);
      }
      titleNeedsRedraw = false;
    }
    for (int i = 0; i < NUM_FALLING_PIECES; i++) {
      drawAnimatedTetromino(fallingPieces[i].x, fallingPieces[i].y, fallingPieces[i].type, fallingPieces[i].rotation, fallingPieces[i].color);
      if (fallingPieces[i].y > tft.height()) { resetPiece(fallingPieces[i]); }
    }
    if (millis() - lastBlinkTime > 500) {
      lastBlinkTime = millis();
      promptVisible = !promptVisible;
      uint16_t promptColor = promptVisible ? ILI9341_WHITE : tft.color565(5, 64, 64);
      tft.setTextSize(PROMPT_SIZE); tft.setTextColor(promptColor);
      tft.setCursor(PROMPT_X_1, PROMPT_Y_1); tft.print("Press Center Key");
      tft.setCursor(PROMPT_X_2, PROMPT_Y_2); tft.print("To Start");
    }
  }
}

// =========================================================================
// ==    MODIFIED: SHOW DIFFICULTY SELECTION SCREEN (w/ Music)            ==
// =========================================================================
// =========================================================================
// ==      MODIFIED: SHOW DIFFICULTY SELECTION SCREEN (Flicker Fixed)     ==
// =========================================================================
void showDifficultyScreen() {
  drawGradientRegion(0, 0, tft.width(), tft.height());
  tft.setTextColor(ILI9341_WHITE);
  
  tft.setTextSize(2);
  tft.setCursor(88, 50);
  tft.print("SELECT LEVEL");
  tft.setTextSize(1);
  tft.setCursor(115, 170);
  tft.print("<-- LEVEL -->");
  tft.setCursor(85, 200);
  tft.print("Press Center to Start");
  
  level = 1;
  int displayedLevel = 0; // NEW: Variable to track the last drawn level

  unsigned long lastMusicNoteTime = 0;
  int musicNoteIndex = 0;

  while (true) {
    // Play non-blocking background music
    unsigned long currentMillis = millis();
    int duration = 1000 / levelSelectDurations[musicNoteIndex];
    if (currentMillis - lastMusicNoteTime > duration) {
        lastMusicNoteTime = currentMillis;
        int note = levelSelectMelody[musicNoteIndex];
        if (note > 0) { tone(SPEAKER_PIN, note, duration * 0.9); }
        musicNoteIndex++;
        if (musicNoteIndex >= (sizeof(levelSelectMelody)/sizeof(int))) { musicNoteIndex = 0; }
    }

    // FIX: Only redraw the number if the level has changed
    if (level != displayedLevel) {
      tft.setTextSize(4);
      tft.fillRect(124, 100, 72, 40, tft.color565(0, 0, map(120, 0, tft.height()-1, 80, 0)));
      if (level < 10) tft.setCursor(142, 108);
      else tft.setCursor(124, 108);
      tft.print(level);
      displayedLevel = level; // Update the tracker
    }

    char key = getKey();
    if (key != ' ') {
        switch (key) {
          case 'L':
            level--;
            if (level < 1) level = 1;
            playMoveSound();
            break;
          case 'R':
            level++;
            if (level > 15) level = 15;
            playMoveSound();
            break;
          case 'S':
            noTone(SPEAKER_PIN);
            return;
        }
    }
  }
}

// =========================================================================
// ==           NEW: SHOW HOW TO PLAY SCREEN WITH COUNTDOWN               ==
// =========================================================================
// =========================================================================
// ==       MODIFIED: SHOW HOW TO PLAY SCREEN (Resized Elements)          ==
// =========================================================================
void showControlsScreen() {
  drawGradientRegion(0, 0, tft.width(), tft.height());
  tft.setTextColor(ILI9341_WHITE);

  // Title (Size 2)
  tft.setTextSize(2);
  tft.setCursor(88, 20);
  tft.print("HOW TO PLAY");

  // --- Draw Visual Keypad and Labels (Smaller) ---
  int keySize = 20; // Reduced from 30
  int keySpacing = 4;
  int centerX = 100;
  int centerY = 90;
  tft.setTextSize(1); // Reduced from 2

  // Up Key (Rotate)
  tft.drawRect(centerX, centerY - keySize - keySpacing, keySize, keySize, ILI9341_WHITE);
  tft.setCursor(centerX + 10, centerY - keySize - keySpacing + 9);
  tft.print("^");
  tft.setCursor(centerX + keySize + 20, centerY - keySize - keySpacing + 9);
  tft.print("ROTATE");

  // Left Key (Move Left)
  tft.drawRect(centerX - keySize - keySpacing, centerY, keySize, keySize, ILI9341_WHITE);
  tft.setCursor(centerX - keySize - keySpacing + 10, centerY + 9);
  tft.print("<");
  tft.setCursor(centerX + keySize + 20, centerY + 9);
  tft.print("MOVE LEFT");
  
  // Right Key (Move Right)
  tft.drawRect(centerX + keySize + keySpacing, centerY, keySize, keySize, ILI9341_WHITE);
  tft.setCursor(centerX + keySize + keySpacing + 10, centerY + 9);
  tft.print(">");
  tft.setCursor(centerX - (9 * 6 * 1) - 35, centerY + 9); 
  tft.print("MOVE RIGHT");

  // Down Key (Soft Drop)
  tft.drawRect(centerX, centerY + keySize + keySpacing, keySize, keySize, ILI9341_WHITE);
  tft.setCursor(centerX + 10, centerY + keySize + keySpacing + 9);
  tft.print("v");
  tft.setCursor(centerX + keySize + 20, centerY + keySize + keySpacing + 9);
  tft.print("SOFT DROP");

  // Center Key (Start)
  tft.fillRect(centerX, centerY, keySize, keySize, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(1);
  tft.setCursor(centerX + 4, centerY + 5);
  tft.print("S");
  tft.setTextColor(ILI9341_WHITE);

  // --- Countdown Timer ---
  tft.setTextSize(1.5);
  for (int i = 5; i > 0; i--) {
    tft.fillRect(0, 180, tft.width(), 30, tft.color565(0, 0, map(195, 0, tft.height()-1, 80, 0)));
    tft.setCursor(65, 185);
    tft.print("Game starts in... ");
    tft.print(i);
    tone(SPEAKER_PIN, NOTE_A4, 100);
    delay(1000);
  }
  noTone(SPEAKER_PIN);
  tone(SPEAKER_PIN, NOTE_C5, 200);
}

// =========================================================================
// ==           START GAME FUNCTION (Unchanged)                         ==
// =========================================================================
void startGame() {
  playGameStartSound();
  delay(200);
  currentNote = 0;
  lastNoteTime = millis();
  musicPlaying = true;
  memset(board, 0, sizeof(board));
  score = 0;
  linesCleared = 0;
  gameOver = false;
  
  fallSpeed = max(100, 1050 - (level * 50)); 
  
  drawGameLayout();
  nextPiece = random(7);
  spawnNewPiece();
  updateScoreDisplay();
  drawBoard();
}

// =========================================================================
// ==                             MAIN LOOP                               ==
// =========================================================================
void loop() {
  updateGameMusic();
  if (gameOver) {
    musicPlaying = false;
    noTone(SPEAKER_PIN);
    playGameOverSound();
    
    if (score > highScore) {
      highScore = score;
      EEPROM.put(EEPROM_ADDR, highScore);
      tft.setTextSize(2);
      tft.setTextColor(ILI9341_YELLOW);
      tft.setCursor(BOARD_X_OFFSET - 10, 200);
      tft.print("New High Score!");
    }
    
    tft.setTextSize(3);
    tft.setTextColor(ILI9341_RED);
    tft.setCursor(BOARD_X_OFFSET + 25, 80);
    tft.println("GAME");
    tft.setCursor(BOARD_X_OFFSET + 25, 110);
    tft.println("OVER");
    tft.setTextSize(1);
    tft.setCursor(BOARD_X_OFFSET - 5, 160);
    tft.setTextColor(ILI9341_WHITE);
    tft.println("Press Center Key to Restart");

    while (getKey() != 'S');
    showDifficultyScreen();
    showControlsScreen();
    startGame();
    return;
  }
  handleInput();
  updateGame();
}

// =========================================================================
// ==                        GAME LOGIC & DRAWING                         ==
// =========================================================================
void drawBlock(int x, int y, uint16_t color) {
  int16_t px = BOARD_X_OFFSET + x * BLOCK_SIZE;
  int16_t py = BOARD_Y_OFFSET + y * BLOCK_SIZE;
  if (color == 0) { drawGradientRegion(px, py, BLOCK_SIZE, BLOCK_SIZE); }
  else {
    tft.fillRect(px, py, BLOCK_SIZE, BLOCK_SIZE, color);
    tft.drawRect(px, py, BLOCK_SIZE, BLOCK_SIZE, ILI9341_BLACK);
  }
}
void drawBoard() { for (int y = 0; y < BOARD_HEIGHT; y++) { for (int x = 0; x < BOARD_WIDTH; x++) { drawBlock(x, y, COLORS[board[y][x]]); } } }
void drawPiece() { for (int y = 0; y < 4; y++) { for (int x = 0; x < 4; x++) { if (PIECES[currentPiece][currentRotation][y][x]) { drawBlock(currentX + x, currentY + y, COLORS[currentPiece + 1]); } } } }
void erasePiece() { for (int y = 0; y < 4; y++) { for (int x = 0; x < 4; x++) { if (PIECES[currentPiece][currentRotation][y][x]) { drawBlock(currentX + x, currentY + y, 0); } } } }
void drawGameLayout() { drawGradientRegion(0, 0, tft.width(), tft.height()); tft.drawRect(BOARD_X_OFFSET - 2, BOARD_Y_OFFSET - 2, BOARD_WIDTH * BLOCK_SIZE + 4, BOARD_HEIGHT * BLOCK_SIZE + 4, ILI9341_WHITE); }

void handleInput() {
  char key = getKey();
  if (key != ' ') {
    playMoveSound();
    erasePiece();
    switch (key) {
      case 'L': movePiece(-1, 0); break;
      case 'R': movePiece(1, 0); break;
      case 'U': rotatePiece(); break;
      case 'D': movePiece(0, 1); lastFallTime = millis(); break;
    }
    drawPiece();
  }
}
void updateGame() {
  if (millis() - lastFallTime > fallSpeed) {
    erasePiece();
    if (!checkCollision(currentX, currentY + 1, currentRotation)) { currentY++; }
    else { placePiece(); clearLines(); spawnNewPiece(); drawBoard(); }
    drawPiece();
    lastFallTime = millis();
  }
}
void spawnNewPiece() { currentPiece = nextPiece; nextPiece = random(7); currentRotation = 0; currentX = BOARD_WIDTH / 2 - 2; currentY = 0; updateScoreDisplay(); if (checkCollision(currentX, currentY, currentRotation)) { gameOver = true; } }
void placePiece() { for (int y = 0; y < 4; y++) { for (int x = 0; x < 4; x++) { if (PIECES[currentPiece][currentRotation][y][x]) { board[currentY + y][currentX + x] = currentPiece + 1; } } } }
void clearLines() {
  int linesFound = 0;
  for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
    bool lineFull = true;
    for (int x = 0; x < BOARD_WIDTH; x++) { if (board[y][x] == 0) { lineFull = false; break; } }
    if (lineFull) {
      linesFound++;
      for (int moveY = y; moveY > 0; moveY--) { for (int moveX = 0; moveX < BOARD_WIDTH; moveX++) { board[moveY][moveX] = board[moveY - 1][moveX]; } }
      y++;
    }
  }
  if (linesFound > 0) {
    playLineClearSound();
    if (linesFound == 1) score += 40 * level;
    if (linesFound == 2) score += 100 * level;
    if (linesFound == 3) score += 300 * level;
    if (linesFound == 4) score += 1200 * level;
    linesCleared += linesFound;
    if (linesCleared >= level * 10) { level++; fallSpeed = max(100, fallSpeed - 50); }
    updateScoreDisplay();
  }
}
void movePiece(int dx, int dy) { if (!checkCollision(currentX + dx, currentY + dy, currentRotation)) { currentX += dx; currentY += dy; } }
void rotatePiece() {
  int newRotation = (currentRotation + 1) % 4;
  if (!checkCollision(currentX, currentY, newRotation)) { currentRotation = newRotation; }
  else if (!checkCollision(currentX - 1, currentY, newRotation)) { currentX--; currentRotation = newRotation; }
  else if (!checkCollision(currentX + 1, currentY, newRotation)) { currentX++; currentRotation = newRotation; }
}
bool checkCollision(int xPos, int yPos, int rotation) { for (int y = 0; y < 4; y++) { for (int x = 0; x < 4; x++) { if (PIECES[currentPiece][rotation][y][x]) { int boardX = xPos + x; int boardY = yPos + y; if (boardX < 0 || boardX >= BOARD_WIDTH || boardY >= BOARD_HEIGHT) return true; if (boardY >= 0 && board[boardY][boardX]) return true; } } } return false; }

void updateScoreDisplay() {
  scoreDisplay.clearDisplay();
  scoreDisplay.setTextColor(SSD1306_WHITE);
  scoreDisplay.setTextSize(1);
  scoreDisplay.setCursor(2, 2);
  scoreDisplay.print("SCORE");
  scoreDisplay.setTextSize(2);
  scoreDisplay.setCursor(2, 12);
  scoreDisplay.print(score);
  scoreDisplay.setTextSize(1);
  scoreDisplay.setCursor(2, 34);
  scoreDisplay.print("HI-SCORE");
  scoreDisplay.setTextSize(2);
  scoreDisplay.setCursor(2, 44);
  scoreDisplay.print(highScore);
  scoreDisplay.drawFastVLine(64, 0, 64, SSD1306_WHITE);
  scoreDisplay.setTextSize(1);
  scoreDisplay.setCursor(70, 2);
  scoreDisplay.print("LEVEL");
  scoreDisplay.setTextSize(2);
  scoreDisplay.setCursor(82, 12);
  scoreDisplay.print(level);
  scoreDisplay.setTextSize(1);
  scoreDisplay.setCursor(70, 34);
  scoreDisplay.print("NEXT:");
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (PIECES[nextPiece][0][y][x]) {
        scoreDisplay.fillRect(72 + x * 4, 44 + y * 4, 3, 3, SSD1306_WHITE);
      }
    }
  }
  scoreDisplay.display();
}
char getKey() {
  char key = ' ';
  for (int r = 0; r < ROWS; r++) {
    digitalWrite(rowPins[r], LOW);
    for (int c = 0; c < COLS; c++) {
      if (digitalRead(colPins[c]) == LOW) {
        delay(50);
        while (digitalRead(colPins[c]) == LOW);
        key = keys[r][c];
      }
    }
    digitalWrite(rowPins[r], HIGH);
  }
  return key;
}