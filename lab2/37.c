#include <stdio.h>
#include <unistd.h>

int main() {
    execl("/bin/ls", "ls", (char *)NULL);
    printf("What happened?\n");
}