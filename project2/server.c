#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include "Bank.c"
#include "server.h"

int currentReqID = 1;

void main(int argc, char** argv) {
    if (argc != 4) {
        puts("Run the program with args: <# of worker threads> <# of accounts> <output file>");
        return;
    }

    long numThreads = strtol(argv[1], NULL, 0);
    long numAccounts = strtol(argv[2], NULL, 0);
    char* filename = argv[3];

    printf("%ld thread(s), %ld account(s), to %s\n", numThreads, numAccounts, filename);
    initialize_accounts(numAccounts);

    //User input
    char* input = NULL;
    size_t len = 0;

    while(1) {
        puts("Enter a command: ");

        getline(&input, &len, stdin);
        strtok(input, "\n");
        handleRequest(input);
    }

    /////////// PROGRAM END /////////////
    free_accounts();
}

void handleRequest(char* input) {
    char* type = strtok(input, " ");

    if(strcmp(type, "CHECK") == 0) {
        return;
    } else if(strcmp(type, "TRANS") == 0) {
        return;
    } else if(strcmp(type, "END") == 0) {
        puts("Program terminated");
        return;
    }
}