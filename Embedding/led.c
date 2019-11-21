#include <stdio.h>
#include <unistd.h> // close
#include <fcntl.h>  // open
#include <signal.h> // signal functions
#include <stdlib.h>

#define BRIGHT_ON "1"
#define BRIGHT_OFF "0"

volatile sig_atomic_t flag = 1;
static int g_leds_num = 4;
static char *g_leds[] = {
        "/sys/class/leds/user-led0/brightness",
        "/sys/class/leds/user-led1/brightness",
        "/sys/class/leds/user-led2/brightness",
        "/sys/class/leds/user-led3/brightness",
};

static void sig_handler(int sig) { // can be called asynchronously
    flag = 0; // set flag
    fprintf(stderr, "Signal(%d) received, program will be closed.\n", sig);
}

int main(int argc, char *argv[]) {
    int i = 0, j = 0, brights_num = 0;
    int leds[g_leds_num];
    int *brights = NULL;

    if (argc == 1) {
        brights_num = g_leds_num;
        brights = malloc(brights_num);
        for (i = 0; i < brights_num; ++i) {
            brights[i] = i;
        }
    } else {
        brights_num = argc - 1;
        brights = malloc(brights_num);
        for (i = 0; i < brights_num; ++i) {
            j = atoi(argv[i + 1]);
            brights[i] = (j >= 0 && j <= g_leds_num) ? j : -1;
        }
    }

    signal(SIGINT, sig_handler);

    for (i = 0; i < g_leds_num; ++i) {
        leds[i] = open(g_leds[i], O_WRONLY);
        if (leds[i] == -1) {
            fprintf(stderr, "ERR: Led(%d) open error.\n", i);
            for (j = i - 1; j > 0; j--) close(leds[i]);
            return 1;
        }
    }

    flag = 1;
    while (flag) {
        for (i = 0; i < brights_num; i++) {
            write(leds[brights[i]], BRIGHT_ON, sizeof(BRIGHT_ON));
            usleep(500 * 1000);
            write(leds[brights[i]], BRIGHT_OFF, sizeof(BRIGHT_OFF));
            usleep(500 * 1000);
        }
    }

    for (i = 0; i < g_leds_num; ++i) close(leds[i]);
    return 0;
}