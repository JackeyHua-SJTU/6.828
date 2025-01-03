#include "kernel/types.h"
#include "user/user.h"

#define MAX_SIZE 40

// recursive 
// @param A pipe
void prime(int p[]) {
    int recv[MAX_SIZE]; // receive from parent pipe
    close(p[1]);
    int n = 0;
    while (read(p[0], recv + n, sizeof(int)) > 0) ++n;
    close(p[0]);

    if (n == 0) return;
    printf("prime %d\n", recv[0]);
    if (n == 1) return;
    int cur_pipe[2];
    int state = pipe(cur_pipe);
    if (state < 0) {
        printf("pipe: failed to create pipe\n");
        exit(1);
    }
    int pid = fork();
    if (pid < 0) {
        printf("fork: failed to fork a child process\n");
        exit(1);
    }
    if (pid == 0) {
        prime(cur_pipe);
        exit(0);
    } else {
        close(cur_pipe[0]);
        for (int i = 1; i < n; ++i) {
            // printf("[%d]: %d\n", recv[0], recv[i]);
            if (recv[i] % recv[0] != 0) {
                write(cur_pipe[1], &recv[i], sizeof(int));
            }
        }
        close(cur_pipe[1]);
        wait(0);    // wait for child process to end
    }
    exit(0);
}

int main(int argc, char** argv) {
    if (argc != 1) {
        fprintf(2, "primes usage: primes\n");
        exit(1);
    }

    int p[2];
    int s = pipe(p);
    if (s < 0) {
        printf("pipe: failed to create pipe\n");
        exit(1);
    }

    for (int i = 2; i <= 35; ++i) {
        write(p[1], &i, sizeof(int));
    }
    close(p[1]);

    prime(p);

    exit(0);
}