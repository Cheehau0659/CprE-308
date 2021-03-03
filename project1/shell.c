#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

void cmdForeground(char* args[]);
void cmdBackground(char* args[]);
void parseArgs(char* input, char* args[]);
char lastChar(char*);

//TODO free memory leaks, kill processes, comment
int main(int argc, char* argv[]) {
    // Set the prompt	 
    char* prompt;

    if(argc == 3 && strcmp(argv[1], "-p") == 0) {
        prompt = argv[2];
    } else if(argc == 1) {
        prompt = "308sh";
    } else {
        printf("Enter '-p <prompt>' as arguments for a custom prompt");
        return 0;
    }

    // User input infinite loop
    char* input = NULL;
    size_t len = 0;
    while (1) {
        printf("\n%s> ", prompt);

        // Get user input
        getline(&input, &len, stdin);
        strtok(input, "\n");

        // ---------- Builtin Commands ---------------------------------------	 	 
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
                path = strtok(NULL, "\0");

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
            // ---------- Non-Builtin Commands--------------------------
            char* args[20];
            parseArgs(input, args);

            if(lastChar(args[0]) == '&') {
                cmdBackground(args);
            } else {
                cmdForeground(args);
            }
        }
    }
    return 1;
}
// ----------------------------------- Helper Functions ----------------------------------
void cmdForeground(char* args[]) {
    pid_t pid = fork(); // don't work atm
    if(pid == 0) {      
        printf("Child PID: %d", pid);

        if(execvp(args[0], args) == -1) {
            printf("That is not a valid command");
        }
    } else {
        int* stat_loc;
        pid_t child = waitpid(pid, stat_loc, 0);
        int status = *stat_loc;
        printf("Child %d exited with status %d", child, status);
    }
}

void cmdBackground(char* args[]) {
    printf("Background processes not implemented yet");
}

void parseArgs(char* input, char* args[]) {
    int i;
    for(i = 0; i < 20; i++) {
        args[i] = strsep(&input, " ");

        if(args[i] == NULL) {
            break;
        }
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