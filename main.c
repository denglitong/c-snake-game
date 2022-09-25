//
// Created by Litong Deng on 2022/9/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "snake.h"

int main() {
    // set random seed
    srand(time(NULL));
    // clear terminal board to draw the snake graph
    clearBoard();
    setUpKeyBoard();

    // initialize the snake direction and len
    char direction = DOWN;
    int snakeLen = 4, life = 1, score = 0;
    initialize(snakeLen, life, score);

    // start game
    showFood(snakeLen);
    // response to the keyboard input
    respondKeyBoard(&direction, &snakeLen, &life, &score);

    // terminate the game when the life is 0
    gotoxy(1, ROW + 2);
    printf("\n\n\e[31mGame Over!\e[0m Your score is: %d.\n\n", score);

    return 0;
}