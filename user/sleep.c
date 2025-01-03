#include "kernel/types.h"
#include "user/user.h"  // for atoi

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(2, "sleep usage: sleep <time>");
        exit(1);
    }
    int len = atoi(argv[1]);

    int ret = sleep(len);
    if (ret < 0) {
        printf("sleep: can not sleep for %s\n", argv[1]);
        exit(1);
    }
    exit(0);
}