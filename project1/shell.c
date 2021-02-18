#include <stdio.h>

void main() {
    while (1) {
        type_prompt();
        read_command(command, parameters);

        if(fork() != 0) {
            // Parent code
            waitpid(-1, &status, 0);

        } else {
            // Child code
            execve(command, parameters, 0);
        }
    }
}

