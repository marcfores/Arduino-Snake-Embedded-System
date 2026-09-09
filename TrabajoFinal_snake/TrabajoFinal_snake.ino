#include <LedControl.h>

const int numDevices = 4;
LedControl lc = LedControl(51, 52, 53, numDevices); // DIN, CLK, CS, # dispositivos

// Joystick
const int joyX = A0;
const int joyY = A1;
const int joyButton = 7;

// Juego Snake
int snake[256][2]; // Posiciones del cuerpo del snake
int length = 3;
int dirX = 1;  // Dirección inicial derecha
int dirY = 0;
int foodX = 10;
int foodY = 4;
bool game_over = false;
bool paused = false;

// Variables para manejo del botón y debounce
unsigned long lastButtonPress = 0;
const unsigned long debounceDelay = 300;

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < numDevices; i++) {
    lc.shutdown(i, false);
    lc.setIntensity(i, 8);
    lc.clearDisplay(i);
  }

  pinMode(joyButton, INPUT_PULLUP);
  randomSeed(analogRead(A2)); // Para aleatoriedad

  startGame();
  drawScore();
}

void loop() {
  // Manejo del botón para pausa / reanudar
  if (digitalRead(joyButton) == LOW) {
    unsigned long currentMillis = millis();
    if (currentMillis - lastButtonPress > debounceDelay) {
      if (game_over) {
        startGame();
      } else {
        paused = !paused;
        Serial.println(paused ? "Pausa ON" : "Pausa OFF");
      }
      lastButtonPress = currentMillis;
    }
  }

  if (game_over || paused) {
    delay(100);
    return;
  }

  readJoystick();
  moveSnake();

  // Velocidad variable: disminuye delay según longitud de la serpiente, mínimo 50 ms
  int speed = max(50, 200 - (length * 5));
  delay(speed);
}

void startGame() {
  length = 3;
  dirX = 1;
  dirY = 0;
  game_over = false;
  paused = false;

  clearAllDisplays();

  for (int i = 0; i < length; i++) {
    snake[i][0] = 3 - i;  // x
    snake[i][1] = 3;      // y
  }

  spawnFood();
  drawSnake();
  drawScore();
}

void drawSnake() {
  clearAllDisplays();

  for (int i = 0; i < length; i++) {
    setPixel(snake[i][0], snake[i][1], true);
  }

  // Dibuja manzana
  setPixel(foodX, foodY, true);
}

void setPixel(int x, int y, bool state) {
  if (x < 0 || x > 31 || y < 0 || y > 7) return;

  // Mapeo invertido: derecha (M0) a izquierda (M3)
  int matrix = 3 - (x / 8);  // Corrige el orden físico
  int col = x % 8;
  lc.setLed(matrix, y, col, state);
}

void moveSnake() {
  int newX = snake[0][0] + dirX;
  int newY = snake[0][1] + dirY;

  // Colisión con paredes
  if (newX < 0 || newX > 31 || newY < 0 || newY > 7) {
    gameOver();
    return;
  }

  // Colisión con sí mismo
  for (int i = 0; i < length; i++) {
    if (snake[i][0] == newX && snake[i][1] == newY) {
      gameOver();
      return;
    }
  }

  // Mover cuerpo
  for (int i = length; i > 0; i--) {
    snake[i][0] = snake[i - 1][0];
    snake[i][1] = snake[i - 1][1];
  }

  snake[0][0] = newX;
  snake[0][1] = newY;

  // Comer manzana
  if (newX == foodX && newY == foodY) {
    length++;
    spawnFood();
    drawScore();
  }

  drawSnake();
}

void spawnFood() {
  // Buscar todos los espacios libres donde puede aparecer la comida
  int freeSpaces[256][2];
  int freeCount = 0;

  for (int x = 0; x < 32; x++) {
    for (int y = 0; y < 8; y++) {
      bool occupied = false;
      for (int i = 0; i < length; i++) {
        if (snake[i][0] == x && snake[i][1] == y) {
          occupied = true;
          break;
        }
      }
      if (!occupied) {
        freeSpaces[freeCount][0] = x;
        freeSpaces[freeCount][1] = y;
        freeCount++;
      }
    }
  }

  if (freeCount == 0) {
    // ¡El jugador ganó! No hay espacio para comida, termina el juego
    gameOver();
    return;
  }

  int idx = random(0, freeCount);
  foodX = freeSpaces[idx][0];
  foodY = freeSpaces[idx][1];
}

void gameOver() {
  game_over = true;
  paused = false;
  Serial.print("GAME OVER. Puntuacion final: ");
  Serial.println(length - 3);

  // Animación: pantalla llena parpadeando 5 veces rápido
  for (int i = 0; i < 5; i++) {
    for (int d = 0; d < numDevices; d++) {
      for (int row = 0; row < 8; row++) {
        lc.setRow(d, row, 0xFF); // todo encendido
      }
    }
    delay(150);
    clearAllDisplays();
    delay(150);
  }
}

void clearAllDisplays() {
  for (int i = 0; i < numDevices; i++) lc.clearDisplay(i);
}

void drawScore() {
  Serial.print("Puntuacion: ");
  Serial.println(length - 3);
}

void readJoystick() {
  int xVal = analogRead(joyX);
  int yVal = analogRead(joyY);

  // Movimiento horizontal
  if (xVal < 300 && dirX != 1) {
    dirX = -1; dirY = 0;
  } else if (xVal > 700 && dirX != -1) {
    dirX = 1; dirY = 0;
  }

  // Movimiento vertical
  if (yVal < 300 && dirY != 1) {
    dirX = 0; dirY = -1;
  } else if (yVal > 700 && dirY != -1) {
    dirX = 0; dirY = 1;
  }
}
