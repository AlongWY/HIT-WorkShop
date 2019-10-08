//
// Created by along on 17-12-28.
//

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