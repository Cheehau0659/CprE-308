#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

int nonBuiltIn(char input[]);

const char* prompt;

int main(int argc, char *argv[]) {
    // Set the prompt	 
    if(argc == 3 && strcmp(argv[1], "-p") == 0) {
        prompt = argv[2];
    } else if(argc == 1) {
        prompt = "308sh";
    } else {
        puts("Enter '-p <prompt>' as arguments for a custom prompt");
        return 0;
    }
    printf("%s> ", prompt);

    // User input infinite loop
    char* input;
    while (1) {
        // Get user input
        scanf("%s", input);
        
        // ---------- Builtin Commands ---------------------------------------	 	 
        if (strcmp(input, "exit") == 0) {
            return 0;
        } else if (strcmp(input, "pid") == 0) {
            printf("Shell Process ID: %d\n", getpid());
        } else if (strcmp(input, "ppid") == 0) {
            //printf("Parent Process ID: %d\n", getppid());
        } else if (strcmp(input, "pwd") == 0) {
            //printf("Current working directory: %d\n", getcwd());
        } else {
            // ---------- Non-Builtin Commands--------------------------
            nonBuiltIn(input);
        }
        printf("%s> ", prompt);
    }
    return 0;
}
// ----------------------------------- Helper Functions ----------------------------------
int nonBuiltIn(char input[]) {
    return 0;
}