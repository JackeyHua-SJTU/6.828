#include "kernel/types.h"
#include "user/user.h"

#define MAX_SIZE 10

int main(int argc, char** argv) {
    if (argc != 1) {
        fprintf(2, "pingpong usage: pingpong");
        exit(1);
    }

    int child_to_parent[2], parent_to_child[2];
    if (pipe(child_to_parent) < 0 || pipe(parent_to_child) < 0) {
        printf("pipe: failed to create a pipe\n");
        exit(1);
    }
    int pid = fork();
    if (pid < 0) {
        printf("fork: failed to fork a child process\n");
        exit(1);
    }

    if (pid == 0) {
        char buf[MAX_SIZE];
        close(parent_to_child[1]);
        close(child_to_parent[0]);
        int n = read(parent_to_child[0], buf, MAX_SIZE - 1);
        if (n < 0) {
            printf("read: failed to read from the pipe\n");
            exit(1);
        }
        printf("%d: received ping\n", getpid());
        close(parent_to_child[0]);
        memset(buf, 0, MAX_SIZE);
        buf[0] = 'a';
        n = write(child_to_parent[1], buf, 1);
        if (n != 1) {
            printf("write: failed to write to the pipe\n");
            exit(1);
        }
        close(child_to_parent[1]);
        exit(0);
    } else {
        char buf[MAX_SIZE];
        close(parent_to_child[0]);
        close(child_to_parent[1]);
        strcpy(buf, "p");
        int n = write(parent_to_child[1], buf, strlen(buf));
        if (n != strlen(buf)) {
            printf("write: failed to write to the pipe\n");
            exit(1);
        }
        close(parent_to_child[1]);
        n = read(child_to_parent[0], buf, MAX_SIZE - 1);
        if (n < 0) {
            printf("read: failed to read from the pipe\n");
            exit(1);
        }
        close(child_to_parent[0]);
        printf("%d: received pong\n", getpid());
    }

    exit(0);
}