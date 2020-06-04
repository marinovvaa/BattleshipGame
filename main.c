#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>

#define M 12
#define N 12
#define LAYOUT "layout.syscall"
#define TOURS "tour.syscall"

typedef struct point
{
    int x, y;
}Point;

void print_board(int m, int n, char board[m][n]);
void setup_board(int m, int n, char board[m][n]);
bool has_ships(int m, int n, char board[m][n]);
void shoot(int m, int n, char board[m][n]);
void print_board(int m, int n, char board[m][n])
{
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            printf("%3c", board[i][j]);
        }
        putchar('\n');
    }
}


int main() {
    if (!(M && N)) {
        printf("0 as board dimension doesn't make sense.");
        return 0;
    }
    if (access(LAYOUT, F_OK) == -1) {
        printf("%s doesn't exist.", LAYOUT);
    }
    if (access(TOURS, F_OK) == -1) {
        printf("%s doesn't exist.", TOURS);
    }
    char (*board)[N] = malloc(sizeof(char[M][N]));
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) board[i][j] = '~';
    }
    setup_board(M, N, board);
    printf("Board set up correctly. Let's play!!\n");
    shoot(M, N, board);
    printf("All of the ships have been sunk. Thank you for playing!");
    free(board);
    board = NULL;
    return 0;
}

void setup_board(int m, int n, char board[m][n]) {
    int fd = open(LAYOUT, O_RDONLY);
    if (fd < 0) {
        printf("Error opening file or doesn't exist.");
        return;
    }
    while(1)
    {
        Point coords;
        if (read(fd, &coords, sizeof(Point)) == 0) break;
        else {
            if (coords.x < M && coords.y < N - 1 && board[coords.x][coords.y] == '~') {
                board[coords.x][coords.y] = board[coords.x][coords.y + 1] = '$';
            }
        }
    }
    close(fd);
}

bool has_ships(int m, int n, char board[m][n]) {
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            if (board[i][j] != '~') return true;
        }
    }
    return false;
}

void shoot(int m, int n, char board[m][n]) {
    int fd = open(TOURS, O_WRONLY|O_APPEND);
    while(has_ships(M, N, board)) {
        int x, y;
        printf("Enter x coord: \n");
        scanf("%d", &x);
        printf("Enter y coord: \n");
        scanf("%d", &y);
        if (board[x][y] == '$') {
            printf("Hit! The ship associated with these coordinates has been sunk.\n");
            board[x][y] = '~';
            if (!y && board[x][y + 1] == '$') {
                board[x][y + 1] = '~';
            }
            if (y < N - 1 && board[x][y + 1] == '$') {
                board[x][y + 1] = '~';
            }
            if (y > 0 && board[x][y - 1] == '$') {
                board[x][y - 1] = '~';
            }
            if (write(fd, "A ship has been sunk.", 22) < 0)
            {
                printf("Couldn't write to file.");
                exit(1);
            }
        }
        else {
            printf("You missed. Try again.\n");
            if (write(fd, "Miss.", 6) < 0)
            {
                printf("Couldn't write to file.");
                exit(1);
            }
        }
    }
    close(fd);
}
