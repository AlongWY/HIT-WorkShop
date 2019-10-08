#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[], char *env[]) {
    if (execve("/bin/ls", argv, env) == -1) {
        fprintf(stderr, "execve error\n");
        exit(EXIT_FAILURE);
    }
}
