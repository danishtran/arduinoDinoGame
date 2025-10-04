#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_CS     10
#define TFT_DC      8
#define TFT_RST     9
#define TFT_BLK     7

#define BTN_UP      3
#define BTN_DOWN    2

#define SCREEN_WIDTH   240
#define SCREEN_HEIGHT  240

#define DINO_SIZE  35

unsigned long score = 0;
unsigned long lastScoreUpdate = 0;

int dinoX = SCREEN_WIDTH / 4 - DINO_SIZE / 2;
const int groundY = (3 * SCREEN_HEIGHT) / 4 - DINO_SIZE / 2;
int dinoY = groundY;

bool isJumping = false;
float jumpVelocity = 0;
const float gravity = 0.5;
const unsigned long jumpInterval = 30;
unsigned long lastJumpUpdate = 0;

bool isDucking = false;

float obstacleX = SCREEN_WIDTH;
int obstacleY = groundY - DINO_SIZE + 25;
int obstacleWidth = 15;
int obstacleHeight = 20;
uint16_t obstacleColor = ST77XX_YELLOW;

float obstacleSpeed = 1.5;

bool gameOverFlag = false;

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  tft.init(SCREEN_WIDTH, SCREEN_HEIGHT);
  tft.fillScreen(ST77XX_BLACK);

  pinMode(TFT_BLK, OUTPUT);
  digitalWrite(TFT_BLK, HIGH);

  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);

  drawGroundLine();
  drawDino(dinoX, dinoY, DINO_SIZE, ST77XX_WHITE);

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  drawScore();
}

void loop() {
  bool upPressed = digitalRead(BTN_UP) == LOW;
  bool downPressed = digitalRead(BTN_DOWN) == LOW;

  if (gameOverFlag) {
    if (upPressed || downPressed) {
      resetGame();
    }
    return;
  }

  if (upPressed && !isJumping && !isDucking) {
    isJumping = true;
    jumpVelocity = -8;
  }

  if (downPressed && !isDucking && !isJumping) {
    isDucking = true;
    duck();
  } else if (!downPressed && isDucking) {
    isDucking = false;
    standUp();
  }

  updateJump();
  updateObstacle();

  if (millis() - lastScoreUpdate >= 100) {
    score++;
    lastScoreUpdate = millis();
    drawScore();
  }

  obstacleSpeed = 1.5 + (score / 5000.0);

  if (checkCollision()) {
    gameOver();
  }
}

// Draws Dino
void drawDino(int x, int y, int size, uint16_t color) {
  tft.fillRect(x, y, DINO_SIZE, size, color);
}

// Draws Score
void drawScore() {
  tft.fillRect(SCREEN_WIDTH - 80, 0, 80, 40, ST77XX_BLACK);
  tft.setCursor(SCREEN_WIDTH - 70, 2);
  tft.print(score);
}

// Draws Obstacle
void drawObstacle(int x, int y, int w, uint16_t color) {
  tft.fillRect(x, y, w, obstacleHeight, color);
}

// Draws Ground
void drawGroundLine() {
  int groundLineY = groundY + DINO_SIZE;
  tft.drawLine(0, groundLineY, SCREEN_WIDTH, groundLineY, ST77XX_WHITE);
}

// Makes Dino Duck Down
void duck() {
  drawDino(dinoX, dinoY, DINO_SIZE, ST77XX_BLACK);
  drawDino(dinoX, groundY + (DINO_SIZE / 2), DINO_SIZE / 2, ST77XX_WHITE);
}

// Redraw Dino Standing up
void standUp() {
  drawDino(dinoX, groundY + DINO_SIZE / 2, DINO_SIZE / 2, ST77XX_BLACK);
  drawDino(dinoX, groundY, DINO_SIZE, ST77XX_WHITE);
}

// Jump Dino Function
void updateJump() {
  if (!isJumping) return;

  unsigned long now = millis();
  if (now - lastJumpUpdate > jumpInterval) {
    lastJumpUpdate = now;

    drawDino(dinoX, dinoY, DINO_SIZE, ST77XX_BLACK);

    dinoY += jumpVelocity;
    jumpVelocity += gravity;

    if (dinoY >= groundY) {
      dinoY = groundY;
      isJumping = false;
      jumpVelocity = 0;
    }

    drawDino(dinoX, dinoY, DINO_SIZE, ST77XX_WHITE);
  }
}

// Moves Obstacle Right to Left
void updateObstacle() {
  drawObstacle(obstacleX, obstacleY, obstacleWidth, ST77XX_BLACK);

  obstacleX -= obstacleSpeed;

  if (obstacleX < 1) {
    obstacleX = SCREEN_WIDTH;
    obstacleWidth  = random(20, 50);
    if (random(2) == 0) {
      obstacleY = groundY + DINO_SIZE - 20;
      obstacleColor = ST77XX_GREEN;
    } else {
      obstacleY = groundY - DINO_SIZE + 25;
      obstacleColor = ST77XX_YELLOW;
    }
  }

  drawObstacle(obstacleX, obstacleY, obstacleWidth, obstacleColor);

  drawGroundLine();
}

// Collision Detection
bool checkCollision() {
  int dinoTop = isDucking ? groundY + DINO_SIZE / 2 : dinoY;
  int dinoHeight = isDucking ? DINO_SIZE / 2 : DINO_SIZE;

  int dinoLeft = dinoX;
  int dinoRight = dinoX + DINO_SIZE;
  int dinoBottom = dinoTop + dinoHeight;

  // Obstacle hitbox
  int obsLeft = obstacleX;
  int obsRight = obstacleX + obstacleWidth;
  int obsTop = obstacleY;
  int obsBottom = obstacleY + obstacleHeight;

  bool overlapX = (dinoRight > obsLeft) && (dinoLeft < obsRight);
  bool overlapY = (dinoBottom > obsTop) && (dinoTop < obsBottom);

  return overlapX && overlapY;
}

// Game Over Screen
void gameOver() {
  gameOverFlag = true;

  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(3);
  tft.setCursor(40, 80);
  tft.setTextColor(ST77XX_RED);
  tft.print("GAME OVER");

  tft.setTextSize(2);
  tft.setCursor(60, 130);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("Score: ");
  tft.print(score);

  tft.setCursor(20, 180);
  tft.print("Press any button");
}

// Reset Game
void resetGame() {
  score = 0;
  obstacleSpeed = 1.5;
  obstacleX = SCREEN_WIDTH;
  dinoY = groundY;
  isJumping = false;
  isDucking = false;
  gameOverFlag = false;
  tft.fillScreen(ST77XX_BLACK);

  drawGroundLine();
  drawDino(dinoX, dinoY, DINO_SIZE, ST77XX_WHITE);
  drawScore();
}
