#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void handler() {
    puts("Caught a SIGFPE");
    exit(1);
}

void main() {
    signal(SIGFPE, handler);
    int a = 4;
    a = a / 0;
}