//
// Created by Litong Deng on 2022/9/25.
// Reference: https://blog.csdn.net/charloy/article/details/106737038
//

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "snake.h"

// locations of snake
struct Coordinates snake[MAX_SNAKE_LEN];

// location of previous snake tail
struct Coordinates prevTail;

// location of food generated randomly
struct Coordinates food;

// terminal settings
struct termios oldTerminalSettings, newTerminalSettings;

void gotoxy(int col, int row) {
    printf("%c[%d;%dH", ESC, row, col);
}

void clearBoard() {
    for (int y = STARTING_ROW - 1; y <= ROW; y++) {
        for (int x = 1; x <= COL; x++) {
            gotoxy(x, y);
            printf(" ");
        }
    }
}

void expandBoard() {
    gotoxy(COL, ROW);
    printf("\n");
}

void cleanUpKeyBoard() {
    tcsetattr(0, TCSAFLUSH, &oldTerminalSettings);
}

//  https://stackoverflow.com/questions/29723205/how-to-run-program-whilst-listening-for-user-input-in-c
void setUpKeyBoard() {
    tcgetattr(0, &oldTerminalSettings);
    newTerminalSettings = oldTerminalSettings;

    newTerminalSettings.c_lflag &= ~ICANON; // disable line-at-a-time input
    newTerminalSettings.c_lflag &= ~ECHO; // disable echo
    newTerminalSettings.c_cc[VMIN] = 0; // don't wait for characters
    newTerminalSettings.c_cc[VTIME] = 0; // no minimum wait time

    if (tcsetattr(0, TCSAFLUSH, &newTerminalSettings) == 0) {
        atexit(cleanUpKeyBoard);
    } else {
        fprintf(stderr, "Unable to set terminal mode\n");
        exit(1);
    }
}

char getInputKey() {
    char c;
    if (read(STDIN_FILENO, &c, 1) == 0) {
        return '\0';
    } else {
        return c;
    }
}

void initialize(int snakeLen, int life, int score) {
    // 打印游戏边界 - 横向边界
    for (int i = 1; i <= COL; ++i) {
        gotoxy(i, STARTING_ROW);
        printf(WALL);
        gotoxy(i, ROW);
        printf(WALL);
    }
    // 打印游戏边界 - 竖向边界
    for (int i = STARTING_ROW + 1; i < ROW; ++i) {
        gotoxy(1, i);
        printf(WALL);
        gotoxy(COL, i);
        printf(WALL);
    }

    // 打印 score 和 life
    gotoxy(SCORE_COL, SCORE_ROW);
    // 输出绿色的score
    printf("\e[32mscore: %3d\e[0m", score);

    gotoxy(LIFE_COL, LIFE_ROW);
    printf("\e[32mlife: %3d\e[0m", life);

    // 初始化蛇身
    snake[0].y = ROW / 2;
    snake[0].x = COL / 2;

    // 初始化蛇身为竖向
    for (int i = 0; i < snakeLen; ++i) {
        // 记录蛇身的坐标
        snake[i].x = snake[0].x;
        snake[i].y = snake[0].y + i;
        // 打印蛇形
        gotoxy(snake[i].x, snake[i].y);
        if (i == snakeLen - 1) {
            printf(SNAKE_HEAD); // 蛇头
        } else {
            printf(SNAKE_BODY); // 蛇身
        }
    }
    expandBoard();
}

void movingSnake(char direction, int snakeLen) {
    // 记录原蛇尾位置
    prevTail.x = snake[0].x;
    prevTail.y = snake[0].y;

    // 清除原来的蛇尾
    gotoxy(snake[0].x, snake[0].y);
    printf(" ");

    //原来的蛇头变成现在的蛇身
    gotoxy(snake[snakeLen - 1].x, snake[snakeLen - 1].y);
    printf(SNAKE_BODY);

    // 更新蛇身的坐标，原来蛇尾的坐标不再需要，倒数第二位置的蛇身变为蛇尾
    for (int i = 0; i < snakeLen - 1; ++i) {
        snake[i].x = snake[i + 1].x;
        snake[i].y = snake[i + 1].y;
    }

    // 更新新蛇头的位置
    switch (direction) {
        // the previous DOWN direction only allows DOWN/LEFT/RIGHT direction
        case UP:
            snake[snakeLen - 1].y -= 1;
            break;
        case DOWN:
            snake[snakeLen - 1].y += 1;
            break;
        case LEFT:
            snake[snakeLen - 1].x -= 1;
            break;
        case RIGHT:
            snake[snakeLen - 1].x += 1;
            break;
        default:
            gotoxy(COL, ROW);
            printf("Unknown direction: %c\n", direction);
            // TODO warning
            break;
    }
    // 打印新蛇头
    gotoxy(snake[snakeLen - 1].x, snake[snakeLen - 1].y);
    printf(SNAKE_HEAD);
}

void showFood(int snakeLen) {
    int x, y;
    bool coincide = false;
    // 行: [5, 23], 即[STARTING_ROW+1, ROW-1]
    y = rand() % (ROW - 1 - (STARTING_ROW + 1)) + (STARTING_ROW + 1);
    // 列: [2, 79], 即[2, COL-1]
    x = rand() % (COL - 1 - 2) + 2;

    // 检测生成的食物坐标，是否和蛇身坐标重合
    for (int i = 0; i < snakeLen; ++i) {
        if (x == snake[1].x && y == snake[i].y) {
            coincide = true;
        }
    }

    if (coincide) {
        // 如果重合，则重新生成
        showFood(snakeLen);
    } else {
        food.x = x;
        food.y = y;
        gotoxy(food.x, food.y);
        printf(FOOD);
    }
}

// 判断是否撞墙或者撞到蛇身
void isKill(int *life, int snakeLen) {
    // 检测当前蛇身的坐标是否位于边界位置
    // 不管多少蛇身位于边界，一次循环只减一分
    for (int i = 0; i < snakeLen; ++i) {
        if (snake[i].x == 1 || snake[i].x == COL ||
            snake[i].y == STARTING_ROW || snake[i].y == ROW) {
            *life -= 1;
            gotoxy(LIFE_COL, LIFE_ROW);
            printf("\e[32mlife: %3d\e[0m", *life);
            break;
        }
    }
    // 检测蛇身坐标是否重合
    for (int i = 0; i < snakeLen; ++i) {
        for (int j = i + 1; j < snakeLen; ++j) {
            if (snake[i].x == snake[j].x && snake[i].y == snake[j].y) {
                *life -= 1;
                gotoxy(LIFE_COL, LIFE_ROW);
                printf("\e[32mlife: %3d\e[0m", *life);
                break;
            }
        }
    }
}

void isFoodEaten(int *snakeLen, int *score) {
    for (int i = 0; i < *snakeLen; i++) {
        if (food.x == snake[i].x && food.y == snake[i].y) {
            // 食物被吃
            gotoxy(prevTail.x, prevTail.y);
            printf(SNAKE_BODY);
            // 更改蛇身坐标
            for (int j = *snakeLen; j > 0; j--) {
                snake[j].x = snake[j - 1].x;
                snake[j].y = snake[j - 1].y;
            }
            snake[0].x = prevTail.x;
            snake[0].y = prevTail.y;

            *score += 1;
            *snakeLen += 1;
            gotoxy(SCORE_COL, SCORE_ROW);
            printf("\e[32mscore: %3d\e[0m", *score);

            showFood(*snakeLen);
            break;
        }
    }
}

void respondKeyBoard(char *direction, int *snakeLen, int *life, int *score) {
    char inputKey;

    for (; *life > 0;) {
        // 暂停 200ms
        usleep(200 * 1000);
        // 玩家按一次上下左右的方向键，会读取到三个数字，只有第三个数字可以用来区分不同的方向键
        if ((inputKey = getInputKey()) != '\0') {
            inputKey = getInputKey();
            inputKey = getInputKey();
            inputKey = toupper(inputKey);

            if (inputKey == ARROW_UP && *direction != DOWN) {
                *direction = UP;
            } else if (inputKey == ARROW_LEFT && *direction != RIGHT) {
                *direction = LEFT;
            } else if (inputKey == ARROW_RIGHT && *direction != LEFT) {
                *direction = RIGHT;
            } else if (inputKey == ARROW_DOWN && *direction != UP) {
                *direction = DOWN;
            }
        }

        expandBoard();

        // 将新的方向传递给 movingSnake，让蛇身按照这个方向移动一次
        movingSnake(*direction, *snakeLen);
        // 暂停 10ms
        usleep(10 * 1000);
        // 蛇身移动之后，检测蛇身是否撞墙或撞上自己
        isKill(life, *snakeLen);
        // 检测是否吃到食物
        isFoodEaten(snakeLen, score);

        if (isGamePass(*snakeLen)) {
            break;
        }
        expandBoard();
    }
}

int isGamePass(int snakeLen) {
    return snakeLen == MAX_SNAKE_LEN;
}
