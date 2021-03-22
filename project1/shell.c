#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

// Runs command with execvp
void runCommand(char* args[], bool background);
// Parses input line into string array
bool parseArgs(char* input, char* args[]);
// Gets the last char of a string
char lastChar(char*);

int backProcesses = 0;

int main(int argc, char* argv[]) {
    // Set the prompt	 
    char* prompt;

    if(argc == 3 && strcmp(argv[1], "-p") == 0) {
        prompt = argv[2];
    } else if(argc == 1) {
        prompt = "308sh";
    } else {
        printf("\nEnter '-p <prompt>' as arguments for a custom prompt");
        return 0;
    }

    // User input infinite loop
    char* input = NULL;
    size_t len = 0;

    pid_t backPID;
    int backStatus;

    while (1) {
        // Check background processes
        backPID = waitpid(-1, &backStatus, WNOHANG);

        if(backProcesses) {
            if(WIFEXITED(backStatus)) {
                printf("\nChild %d exited with status %d", backPID, WEXITSTATUS(backStatus));   
                backProcesses--; 
            }
        }
        
        printf("\n%s> ", prompt);

        // Get user input
        getline(&input, &len, stdin);
        strtok(input, "\n");

        // Native commands	 
        if (strcmp(input, "exit") == 0) {
            return 0;
        } else if (strcmp(input, "pid") == 0) {
            printf("Shell Process ID: %d", getpid());
        } else if (strcmp(input, "ppid") == 0) {
            printf("Parent Process ID: %d", getppid());

        } else if (strcmp(input, "pwd") == 0) {
            printf("Current working directory: %s", getcwd(NULL, 0));
        } else if (strncmp(input, "cd", 2) == 0) {
            if(strlen(input) > 2) {
                char* path = strtok(input, " ");
                path = strtok(NULL, "\0"); // Grab the path

                if(chdir(path) == 0) {
                    printf("New working directory: %s", getcwd(NULL, 0));
                } else {
                    printf("Path invalid");
                }
            } else {
                chdir(getenv("HOME"));
                printf("Home directory: %s", getcwd(NULL, 0));
            }
        } else {
            // External commands
            char* args[20]; // Arbitrary number for max arguments
            bool background = parseArgs(input, args);

            if(background) {
                runCommand(args, true);
            } else {
                runCommand(args, false);
            }
        }
    }
    return 1;
}
// ----------------------------------- Helper Functions ----------------------------------
void runCommand(char* args[], bool background) {
    pid_t pid = fork();
    int status;

    if(pid == 0) {     
        usleep(50); // I don't know why the child always is faster than the parent, regardless of sleep
        if(execvp(args[0], args) == -1) {
            printf("That is not a valid command\n");
            exit(1);
        }
    } else {
        printf("\nChild PID: %d", pid);
    }
    
    if(!background) {
        while (wait(&status) != pid); // Busy wait for foreground task
        printf("\nChild %d exited with status %d", pid, WEXITSTATUS(status));    
    } else {
        backProcesses++;
    }
}

bool parseArgs(char* input, char* args[]) {
    int i;
    for(i = 0; i < 20; i++) {
        args[i] = strsep(&input, " ");

        if(args[i] == NULL) {
            break;
        }
    }
    if(lastChar(args[0]) == '&') { // Check if background task
        int len = strlen(args[0]);
        args[0][len-1] = '\0';

        return true;
    } else {
        return false;
    }
}

char lastChar(char* str) {
    int i;
    for(i = 0; i < strlen(str); i++) {
        if(i == strlen(str)-1) {
            return str[i];
        }
    }
}