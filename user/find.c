#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// fstat read

char* fmtname(char *path) {
    static char buf[DIRSIZ + 1];
    char *p;    
    // Find first character after last slash.
    for(p = path + strlen(path); p >= path && *p != '/'; p--) {}
    p++;    
    // Return blank-padded name.
    if(strlen(p) >= DIRSIZ) return p;
    memmove(buf, p, strlen(p));
    memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
    return buf;
}

// open path => read the status => check name for file, recursively check for dir
void find(char* src_path, char* target) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;
    if ((fd = open(src_path, O_RDONLY)) < 0) {
        fprintf(2, "open: failed to open dir %s\n", src_path);
        exit(1);
    }
    if (fstat(fd, &st) < 0) {
        fprintf(2, "fstat: failed to fstat %s\n", src_path);
        close(fd);
        exit(1);
    }

    switch (st.type) {
    case T_FILE:
        if (strcmp(target, fmtname(src_path)) == 0) {
            printf("%s\n", src_path);
        }
        break;
    
    case T_DIR:
        if (strlen(src_path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
            printf("ls: path name too long\n");
            close(fd);
            exit(1);
        }
        strcpy(buf, src_path);
        p = buf + strlen(src_path);
        *(p++) = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0) continue;
            // ! Do not iterate to . and ..
            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            find(buf, target);
        }
        
        break;
    }
    close(fd);
}

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(2, "find usage: find <root_dir> <target name>\n");
        exit(1);
    }
    // We need another buffer to format the target
    // Or all the answer after `fmtname` will point to the same address
    char buf[DIRSIZ + 1];
    memmove(buf, fmtname(argv[2]), DIRSIZ);
    buf[DIRSIZ] = 0;
    find(argv[1], buf);

    exit(0);
}