#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
//#include <windows.h>
#include <limits.h>

#define MAX_WIDTH 40
#define MAX_HEIGHT 20
#define MINIMAX_DEPTH 5

void clearScreen() {
    printf("\033[H\033[J");
}

void printBoard(char **board, int width, int height, FILE *file) {
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            printf("| %c ", board[i][j]);
            fprintf(file, "| %c ", board[i][j]);
        }
        printf("|\n");
        fprintf(file, "|\n");
    }
    for (int j = 0; j < width; ++j) {
        if(j < 9){
            printf("  %d ", j+1);
            fprintf(file, "  %d ", j+1);
        } else {
            printf("  %d", j+1);
            fprintf(file, "  %d", j+1);
        }
    }
    printf("\n");
    fprintf(file, "\n");
}

bool isWinningMove(char **board, int width, int height, int row, int col, char player) {
    // horizontal
    for (int i = 0; i <= width - 4; ++i) {
        if (board[row][i] == player && board[row][i+1] == player && board[row][i+2] == player && board[row][i+3] == player)
            return true;
    }

    // vertical
    for (int i = 0; i <= height - 4; ++i) {
        if (board[i][col] == player && board[i+1][col] == player && board[i+2][col] == player && board[i+3][col] == player)
            return true;
    }

    // diagonal (/)
    for (int i = 3; i < height; ++i) {
        for (int j = 0; j <= width - 4; ++j) {
            if (board[i][j] == player && board[i-1][j+1] == player && board[i-2][j+2] == player && board[i-3][j+3] == player)
                return true;
        }
    }

    // diagonal (\)
    for (int i = 3; i < height; ++i) {
        for (int j = 3; j < width; ++j) {
            if (board[i][j] == player && board[i-1][j-1] == player && board[i-2][j-2] == player && board[i-3][j-3] == player)
                return true;
        }
    }

    return false;
}

bool isDraw(char **board, int width, int height) {
    for (int i = 0; i < width; ++i) {
        if (board[0][i] == ' ')
            return false;
    }
    return true;
}

void freeBoard(char **board, int height) {
    for (int i = 0; i < height; ++i) {
        free(board[i]);
    }
    free(board);
}

bool isValidMove(char **board, int width, int col) {
    return col >= 0 && col < width && board[0][col] == ' ';
}

int evaluateWindow(char *window, char player) {
    int score = 0;
    int opponentCount = 0;
    int playerCount = 0;

    char opponent = (player == 'X') ? 'O' : 'X';
    for (int i = 0; i < 4; i++) {
        if (window[i] == player) playerCount++;
        else if (window[i] == opponent) opponentCount++;
    }

    if (playerCount == 4) score += 100;
    else if (playerCount == 3 && opponentCount == 0) score += 10;
    else if (playerCount == 2 && opponentCount == 0) score += 5;
    else if (opponentCount == 3 && playerCount == 0) score -= 8;

    return score;
}

int scorePosition(char **board, int width, int height, char player) {
    int score = 0;

    int centerCol = width / 2;
    int centerCount = 0;
    for (int row = 0; row < height; row++) {
        if (board[row][centerCol] == player) centerCount++;
    }
    score += centerCount * 3;

    for (int row = 0; row < height; row++) {
        for (int col = 0; col <= width - 4; col++) {
            char window[4];
            for (int i = 0; i < 4; i++) {
                window[i] = board[row][col + i];
            }
            score += evaluateWindow(window, player);
        }
    }

    for (int col = 0; col < width; col++) {
        for (int row = 0; row <= height - 4; row++) {
            char window[4];
            for (int i = 0; i < 4; i++) {
                window[i] = board[row + i][col];
            }
            score += evaluateWindow(window, player);
        }
    }

    for (int row = 0; row <= height - 4; row++) {
        for (int col = 0; col <= width - 4; col++) {
            char window[4];
            for (int i = 0; i < 4; i++) {
                window[i] = board[row + i][col + i];
            }
            score += evaluateWindow(window, player);
        }
    }

    for (int row = 0; row <= height - 4; row++) {
        for (int col = 0; col <= width - 4; col++) {
            char window[4];
            for (int i = 0; i < 4; i++) {
                window[i] = board[row + 3 - i][col + i];
            }
            score += evaluateWindow(window, player);
        }
    }

    return score;
}

int minimax(char **board, int depth, bool maximizingPlayer, int width, int height, char aiPlayer, char humanPlayer) {
    if (depth == 0 || isDraw(board, width, height)) return scorePosition(board, width, height, aiPlayer) - scorePosition(board, width, height, humanPlayer);

    if (maximizingPlayer) {
        int maxEval = INT_MIN;
        for (int col = 0; col < width; col++) {
            if (isValidMove(board, width, col)) {
                int row;
                for (row = height - 1; row >= 0; row--) {
                    if (board[row][col] == ' ') {
                        board[row][col] = aiPlayer;
                        break;
                    }
                }
                if (isWinningMove(board, width, height, row, col, aiPlayer)) {
                    board[row][col] = ' ';
                    return 1000;
                }
                int eval = minimax(board, depth - 1, false, width, height, aiPlayer, humanPlayer);
                board[row][col] = ' ';
                maxEval = (eval > maxEval) ? eval : maxEval;
            }
        }
        return maxEval;
    } else {
        int minEval = INT_MAX;
        for (int col = 0; col < width; col++) {
            if (isValidMove(board, width, col)) {
                int row;
                for (row = height - 1; row >= 0; row--) {
                    if (board[row][col] == ' ') {
                        board[row][col] = humanPlayer;
                        break;
                    }
                }
                if (isWinningMove(board, width, height, row, col, humanPlayer)) {
                    board[row][col] = ' ';
                    return -1000;
                }
                int eval = minimax(board, depth - 1, true, width, height, aiPlayer, humanPlayer);
                board[row][col] = ' ';
                minEval = (eval < minEval) ? eval : minEval;
            }
        }
        return minEval;
    }
}

int getBestMove(char **board, int width, int height, char aiPlayer, char humanPlayer) {
    int bestScore = INT_MIN;
    int bestCol = -1;
    for (int col = 0; col < width; col++) {
        if (isValidMove(board, width, col)) {
            int row;
            for (row = height - 1; row >= 0; row--) {
                if (board[row][col] == ' ') {
                    board[row][col] = aiPlayer;
                    break;
                }
            }
            if (isWinningMove(board, width, height, row, col, aiPlayer)) {
                board[row][col] = ' ';
                return col;
            }
            int score = minimax(board, MINIMAX_DEPTH, false, width, height, aiPlayer, humanPlayer);
            board[row][col] = ' ';
            if (score > bestScore) {
                bestScore = score;
                bestCol = col;
            }
        }
    }
    return bestCol;
}

int main() {
    int width, height, gameMode;
    char filename[100];
    printf("Enter the filename to save the game result: ");
    scanf("%s", filename);
    printf("Enter the width of the board (max %d): ", MAX_WIDTH);
    scanf("%d", &width);
    printf("Enter the height of the board (max %d): ", MAX_HEIGHT);
    scanf("%d", &height);
    printf("Enter game mode (1 for PvP, 2 for PvAI): ");
    scanf("%d", &gameMode);

    if (width < 4 || height < 4 || width > MAX_WIDTH || height > MAX_HEIGHT) {
        printf("Invalid board size.\n");
        return 1;
    }

    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error opening file.\n");
        return 1;
    }

    fprintf(file, "Board height - %d \nBoard width - %d\n", height, width);

    char **board = (char **)malloc(height * sizeof(char *));
    for (int i = 0; i < height; ++i) {
        board[i] = (char *)malloc(width * sizeof(char));
        memset(board[i], ' ', width);
    }

    bool game_over = false;
    int currentPlayer = 1;
    char players[2] = {'X', 'O'};
    while (!game_over) {
        clearScreen();
        printBoard(board, width, height, file);

        int col;
        if (gameMode > 2 || gameMode < 1){ 
            printf("Game mode is invalid.");
            break;
        }
        if (gameMode == 2 && currentPlayer == 2) {
            col = getBestMove(board, width, height, players[1], players[0]);
            printf("AI chooses column %d\n", col + 1);
            sleep(2);
        } else {
            printf("Player %d (%c), enter column (1-%d): ", currentPlayer, players[currentPlayer-1], width);
            scanf("%d", &col);
            col--;
        }

        if (!isValidMove(board, width, col)) {
            printf("Invalid move. Try again.\n");
            fprintf(file, "Player %d (%c) tried making an invalid move\n", currentPlayer, players[currentPlayer-1]);
            sleep(3);
            continue;
        }

        int row;
        for (row = height - 1; row >= 0; row--) {
            if (board[row][col] == ' ') {
                board[row][col] = players[currentPlayer-1];
                break;
            }
        }

        fprintf(file, "Player %d (%c) placed at column %d\n", currentPlayer, players[currentPlayer-1], col + 1);

        if (isWinningMove(board, width, height, row, col, players[currentPlayer-1])) {
            clearScreen();
            printBoard(board, width, height, file);
            printf("Player %d (%c) wins!\n", currentPlayer, players[currentPlayer-1]);
            fprintf(file, "Player %d (%c) wins!\n", currentPlayer, players[currentPlayer-1]);
            game_over = true;
        } else if (isDraw(board, width, height)) {
            clearScreen();
            printBoard(board, width, height, file);
            printf("The game is a draw!\n");
            fprintf(file, "The game is a draw!\n");
            game_over = true;
        } else {
            currentPlayer = 3 - currentPlayer;
        }
    }

    fclose(file);
    freeBoard(board, height);
    return 0;
}
