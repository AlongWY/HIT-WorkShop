---
title: 第八章
author: 冯云龙
date: 2017/12/16
geometry: margin=2cm
documentclass: ctexart
output: pdf_document
---

# 8.12

!ditaa(process runtime)(Process runtime)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                        c
                    +-------+---------+
                    |    "hello"   "hello"
                    |    
                c   |   p
            +-------+-------+---------+
            |     fork   "hello"   "hello"
            |
            |           c
            |       +-------+---------+
            |       |    "hello"   "hello"
            |       |    
            |   p   |   p    
     +------+-------+-------+---------+
    main  fork    fork   "hello"   "hello"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

8个。

# 8.16
2。

# 8.20
```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[], char *env[]) {
    if (execve("/bin/ls", argv, env) == -1) {
        fprintf(stderr, "execve error\n");
        exit(EXIT_FAILURE);
    }
}
```

# 8.24
```c
#include "csapp.h"
#define N 2
#define MAXLEN (50)

int main() {
    int status, i;
    pid_t pid;

    /** Parent creates N children */
    for (i = 0; i < N; ++i) {
        if ((pid = Fork()) == 0) {
            char *cp = "Hello";
            // 产生一个错误
            cp[0] = 'h';
        }
    }

    /** Parent reaps N children in no particalar order */
    while ((pid = waitpid(-1, &status, 0)) > 0) {
        if (WIFCONTINUED(status)) {
            printf("child %d terminated normally with exit status=%d\n", pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            char buf[MAXLEN];
            // 添加代码以打印相关错误
            sprintf(buf, "child %d terminated by signal %d", pid, WTERMSIG(status));
            psignal(WTERMSIG(status), buf);
        } else {
            printf("child %d terminated abnormally\n", pid);
        }
    }

    /** The only normal termination is if there are no more children */
    if (errno != ECHILD)
        unix_error("waitpid error");

    exit(EXIT_SUCCESS);
}
```