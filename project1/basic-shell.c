#include <stdio.h>

void main() {
    while(1) {
        type_prompt();
        read_command(command, parameters);

        if(fork() != 0) {
            waitpid(-1, &status, 0);
        } else {
            execve(command, parameters, 0);
        }
    } 
}