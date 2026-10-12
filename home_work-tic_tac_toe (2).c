#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>


#define MIN_SIZE 3
#define MAX_SIZE 20

#define MODE_COMPUTER 1
#define MODE_TWO_PLAYERS 2

#define EMPTY '-'


int readNumber(const char *prompt, int *number)
{
    char line[100];

    while (1) {
        printf("%s", prompt);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            return 0;
        }

        if (strchr(line, '\n') == NULL && !feof(stdin)) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {
            }
            printf("Ошибка: слишком длинная строка. Попробуйте ещё раз.\n");
            continue;
        }

        char *end;
        errno = 0;
        long value = strtol(line, &end, 10);

        int noDigits = (end == line);

        while (isspace((unsigned char)*end)) {
            end++;
        }

        if (noDigits || *end != '\0' || errno == ERANGE ||
            value < -1000000 || value > 1000000) {
            printf("Ошибка: нужно ввести целое число. Попробуйте ещё раз.\n");
            continue;
        }

        *number = (int)value;
        return 1;
    }
}

char **createBoard(int size)
{
    char **board = malloc(size * sizeof(char *));
    if (board == NULL) {
        return NULL;
    }

    for (int row = 0; row < size; row++) {
        board[row] = malloc(size * sizeof(char));
        if (board[row] == NULL) {
            for (int i = 0; i < row; i++) {
                free(board[i]);
            }
            free(board);
            return NULL;
        }
    }

    return board;
}

void freeBoard(char **board, int size)
{
    for (int row = 0; row < size; row++) {
        free(board[row]);
    }
    free(board);
}

void zeroBoard(char **board, int size)
{
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            board[row][col] = EMPTY;
        }
    }
}

void printEmptyLines()
{
    for (int i = 0; i < 50; i++) {
        printf("\n");
    }
}

void printBoard(char **board, int size)
{
    printf("\n");
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            printf("%c", board[row][col]);
            if (col < size - 1) {
                printf(" | ");
            }
        }
        printf("\n");

        if (row < size - 1) {
            int lineLength = size * 4 - 3;
            for (int i = 0; i < lineLength; i++) {
                printf("-");
            }
            printf("\n");
        }
    }
    printf("\n");
}

int checkW(char **board, int size, char symbol)
{
    for (int row = 0; row < size; row++) {
        int count = 0;
        for (int col = 0; col < size; col++) {
            if (board[row][col] == symbol) {
                count++;
            }
        }
        if (count == size) {
            return 1;
        }
    }

    for (int col = 0; col < size; col++) {
        int count = 0;
        for (int row = 0; row < size; row++) {
            if (board[row][col] == symbol) {
                count++;
            }
        }
        if (count == size) {
            return 1;
        }
    }

    int mainCount = 0;
    for (int i = 0; i < size; i++) {
        if (board[i][i] == symbol) {
            mainCount++;
        }
    }
    if (mainCount == size) {
        return 1;
    }

    int sideCount = 0;
    for (int i = 0; i < size; i++) {
        if (board[i][size - 1 - i] == symbol) {
            sideCount++;
        }
    }
    if (sideCount == size) {
        return 1;
    }

    return 0;
}

int checkD(char **board, int size)
{
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            if (board[row][col] == EMPTY) {
                return 0;
            }
        }
    }
    return 1;
}

int playerMove(char **board, int size, char symbol)
{
    int row;
    int col;

    while (1) {
        if (!readNumber("Введите номер строки: ", &row)) {
            return 0;
        }
        if (!readNumber("Введите номер столбца: ", &col)) {
            return 0;
        }

        if (row < 1 || row > size || col < 1 || col > size) {
            printf("Ошибка: номер строки и столбца должен быть от 1 до %d.\n", size);
            continue;
        }

        row = row - 1;
        col = col - 1;

        if (board[row][col] != EMPTY) {
            printf("Ошибка: эта клетка уже занята. Выберите другую.\n");
            continue;
        }

        board[row][col] = symbol;
        return 1;
    }
}

void computerMove(char **board, int size, char symbol)
{
    int row;
    int col;

    do {
        row = rand() % size;
        col = rand() % size;
    } while (board[row][col] != EMPTY);

    board[row][col] = symbol;
}

int chooseMode()
{
    int mode;

    printf("Выберите режим игры:\n");
    printf("1 - игра против компьютера\n");
    printf("2 - игра вдвоём\n");

    while (1) {
        if (!readNumber("Ваш выбор: ", &mode)) {
            return 0;
        }
        if (mode == MODE_COMPUTER || mode == MODE_TWO_PLAYERS) {
            return mode;
        }
        printf("Ошибка: введите 1 или 2.\n");
    }
}

char chooseSymbol()
{
    int choice;

    printf("\nВыберите, чем будете играть:\n");
    printf("1 - крестики (X)\n");
    printf("2 - нолики (O)\n");

    while (1) {
        if (!readNumber("Ваш выбор: ", &choice)) {
            return 0;
        }
        if (choice == 1) {
            return 'X';
        }
        if (choice == 2) {
            return 'O';
        }
        printf("Ошибка: введите 1 или 2.\n");
    }
}

int chooseSize()
{
    int size;

    printf("\nВведите размер поля (от %d до %d).\n", MIN_SIZE, MAX_SIZE);

    while (1) {
        if (!readNumber("Размер поля: ", &size)) {
            return 0;
        }
        if (size >= MIN_SIZE && size <= MAX_SIZE) {
            return size;
        }
        printf("Ошибка: размер должен быть от %d до %d.\n", MIN_SIZE, MAX_SIZE);
    }
}

int main()
{
#ifdef _WIN32
    /* Включение UTF-8 в консоли Windows, для ру*/
    system("chcp 65001 > nul");
#endif

    srand((unsigned int)time(NULL));

    printf("=== Крестики-нолики ===\n\n");

    int mode = chooseMode();
    if (mode == 0) {
        printf("\nВвод завершён. Программа закрыта.\n");
        return 1;
    }

    char userSymbol = chooseSymbol();
    if (userSymbol == 0) {
        printf("\nВвод завершён. Программа закрыта.\n");
        return 1;
    }
    char otherSymbol = (userSymbol == 'X') ? 'O' : 'X';

    int size = chooseSize();
    if (size == 0) {
        printf("\nВвод завершён. Программа закрыта.\n");
        return 1;
    }

    char **board = createBoard(size);
    if (board == NULL) {
        printf("Ошибка: не удалось выделить память для поля.\n");
        return 1;
    }
    zeroBoard(board, size);

    printf("\n--- Настройки игры ---\n");
    if (mode == MODE_COMPUTER) {
        printf("Режим: игра против компьютера\n");
        printf("Ваш символ: %c, символ компьютера: %c\n", userSymbol, otherSymbol);
    } else {
        printf("Режим: игра вдвоём\n");
        printf("Игрок 1: %c, игрок 2: %c\n", userSymbol, otherSymbol);
    }
    printf("Размер поля: %d x %d\n", size, size);
    printf("Для победы нужно собрать линию длиной %d.\n", size);
    printf("Первыми ходят крестики (X).\n");

    printBoard(board, size);

    char currentSymbol = 'X';

    while (1) {
        int isComputerTurn = (mode == MODE_COMPUTER && currentSymbol == otherSymbol);

        if (isComputerTurn) {
            computerMove(board, size, currentSymbol);
        } else {
            if (mode == MODE_COMPUTER) {
                printf("Ваш ход (%c).\n", currentSymbol);
            } else if (currentSymbol == userSymbol) {
                printf("Ход игрока 1 (%c).\n", currentSymbol);
            } else {
                printf("Ход игрока 2 (%c).\n", currentSymbol);
            }

            if (!playerMove(board, size, currentSymbol)) {
                printf("\nВвод завершён. Программа закрыта.\n");
                freeBoard(board, size);
                return 1;
            }
        }

        printEmptyLines();
        printBoard(board, size);

        if (isComputerTurn) {
            printf("Компьютер (%c) сделал ход.\n", currentSymbol);
        }

        if (checkW(board, size, currentSymbol)) {
            if (currentSymbol == 'X') {
                printf("Победили крестики (X)!\n");
            } else {
                printf("Победили нолики (O)!\n");
            }
            break;
        }

        if (checkD(board, size)) {
            printf("Ничья! Все клетки заполнены.\n");
            break;
        }

        currentSymbol = (currentSymbol == 'X') ? 'O' : 'X';
    }

    freeBoard(board, size);
    printf("Игра окончена.\n");
    return 0;
}
