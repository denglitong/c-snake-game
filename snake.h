//
// Created by Litong Deng on 2022/9/25.
//
// Remember to set your terminal line height and weight to have a better behaviour.
#define ROW 24
#define COL 80
#define STARTING_ROW 4
#define SCORE_ROW (STARTING_ROW-1)
#define SCORE_COL STARTING_ROW
#define LIFE_ROW (STARTING_ROW-1)
#define LIFE_COL (COL-9)
#define MAX_SNAKE_LEN ((ROW-STARTING_ROW-2)*(COL-2))

// ANSI escape code
#define ESC 033
#define SNAKE_HEAD "\e[35m■\e[0m"
#define SNAKE_BODY "■"
#define FOOD SNAKE_BODY
#define WALL SNAKE_BODY

#define UP 'W'
#define DOWN 'S'
#define LEFT 'A'
#define RIGHT 'D'

// Down Arrow  0x1B 0x5B 0x42
// Left Arrow  0x1B 0x5B 0x44
// Right Arrow 0x1B 0x5B 0x43
// Up Arrow    0x1B 0x5B 0x41
#define ARROW_UP 'A'
#define ARROW_DOWN 'B'
#define ARROW_LEFT 'D'
#define ARROW_RIGHT 'C'

/**
 * 0 --> x (col)
 * |
 * y (row)
 */
struct Coordinates {
    int x; // col
    int y; // row
};

void clearBoard();

void expandBoard();

void setUpKeyBoard();

char getInputKey();

void gotoxy(int col, int row);

void initialize(int snakeLen, int life, int score);

void movingSnake(char direction, int snakeLen);

void showFood(int snakeLen);

void isKill(int *life, int snakeLen);

void isFoodEaten(int *snakeLen, int *score);

void respondKeyBoard(char *direction, int *snakeLen, int *life, int *score);

int isGamePass(int snakeLen);
