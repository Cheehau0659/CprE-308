#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include <unistd.h>
#include <sys/wait.h>

#include <string.h>

int nonBuiltIn(char* input, char* argv[]);

//TODO free memory leaks
int main(int argc, char* argv[]) {
    // Set the prompt	 
    char* prompt;

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
    char* input = NULL;
    size_t len = 0;
    while (1) {
        // Get user input
        getline(&input, &len, stdin);
        strtok(input, "\n");
        //puts(input);
        //printf("%lu\n", strlen(input));

        // ---------- Builtin Commands ---------------------------------------	 	 
        if (strcmp(input, "exit") == 0) {
            return 0;
        } else if (strcmp(input, "pid") == 0) {
            printf("Shell Process ID: %d\n", getpid());
        } else if (strcmp(input, "ppid") == 0) {
            printf("Parent Process ID: %d\n", getppid());

        } else if (strcmp(input, "pwd") == 0) {
            printf("Current working directory: %s\n", getcwd(NULL, 0));
        } else if (strncmp(input, "cd", 2) == 0) {
            if(strlen(input) > 2) {
                char* path = strtok(input, " ");
                path = strtok(NULL, "\0");

                if(chdir(path) == 0) {
                    printf("New working directory: %s\n", getcwd(NULL, 0));
                } else {
                    puts("Path invalid");
                }
            } else {
                chdir(getenv("HOME"));
                printf("Home directory: %s\n", getcwd(NULL, 0));
            }
        } else {
            // ---------- Non-Builtin Commands--------------------------
            nonBuiltIn(input, argv);
        }
        printf("%s> ", prompt);
    }
    return 0;
}
// ----------------------------------- Helper Functions ----------------------------------
int nonBuiltIn(char* input, char* argv[]) {
    if(fork() == 0) {    
        char* cmd = strtok(input, " ");   
        execvp(cmd, argv); //wtf
    } else {
        waitpid(-1, NULL, 0);
    }
}