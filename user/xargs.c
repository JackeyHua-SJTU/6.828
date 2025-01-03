#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define MAX_LEN 128

int main(int argc, char** argv) {
    if (argc == 1) {
        fprintf(2, "xargs usage: xargs <normal instruction>\n");
        exit(1);
    }
    // get the whole param list 
    char** args = (char**)malloc(MAXARG * sizeof(char*));
    for (int i = 1; i < argc; ++i) {
        args[i - 1] = argv[i];
    }
    char** p = args + argc - 1;
    char buf[MAX_LEN];
    int n = 0;
    int cnt = 0;
    while (1) {
        // if blank is encountered, move the string to the next param
        // if new line is encountered, perform the instruction
        while (read(0, buf + n, 1) == 1) {
            if (buf[n] == ' ' || buf[n] == '\n') break;
            ++n; 
        } 
        if (n == 0) break;
        int flag = (buf[n] == ' ') ? 1 : 0;
        buf[n] = 0;
        p[cnt] = (char*)malloc(strlen(buf) + 1);
        strcpy(p[cnt], buf);
        p[cnt][strlen(buf)] = 0;
        ++cnt;
        if (flag) {
            n = 0;
            continue;
        }
        // \n is encountered
        int pid = fork();
        if (pid < 0) {
            printf("fork: failed to fork a child process\n");
            exit(1);
        }

        if (pid == 0) {
            exec(args[0], args);
        } else {
            wait(0);
            for (int i = 0; i < cnt; ++i) free(p[i]);
            n = 0;
            cnt = 0;
        }
    }

    free(args);
    exit(0);
}   