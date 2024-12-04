#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define BUFFER_SIZE 512

// read from pipe to buf
char *ugets(char *buf, int max) {
    char c;
    int i;
    for (i = 0; i + 1 < max;) {
        int cc = read(0, &c, 1);      //read a byte
        if (cc < 1)
            break;
        if (c == '\n')
            break;
        buf[i ++] = c; // when meet '\n',it's the next line
    }
    buf[i] = '\0';
    return buf;
}

// get one command from stdio
int getcmd(char *buf, int nbuf) {
    memset(buf, 0, nbuf);
    ugets(buf, nbuf);
    if (buf[0] == 0) // EOF
        return -1;
    return 0;
}

int main(int argc, char *argv[]) 
{
    if (argc < 2) {
        fprintf(2, "usage: xargs command\n");
        exit(1);
    }

    char *_argv[MAXARG];
    int _argc = argc - 1;
    for(int i = 0; i < _argc; i ++) {
        _argv[i] = argv[i + 1];
    }
    char buf[BUFFER_SIZE];

    while(getcmd(buf, sizeof(buf)) != -1) {
        if(fork() == 0) {
            _argv[_argc] = buf;
            exec(argv[1], _argv);
        }
        wait(0);
    }
    exit(0);
}
