#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/time.h>
#include<pthread.h>
#include "Bank.c"
#include "server.h"

int currentReqID = 1;
int isRunning = 1;

queue* transactions;
account* accounts;
pthread_mutex_t queuetex;
FILE* fOut;

void main(int argc, char** argv) {
    int i;

    // catch invalid arguments for program
    if (argc != 4) {
        puts("Run the program with args: <# of worker threads> <# of accounts> <output file>");
        return;
    }

    // initialize, get cmd line args, open file
    transactions = (queue*) malloc(sizeof(queue));
    startQueue();
    pthread_mutex_init(&queuetex, NULL);

    long numThreads = strtol(argv[1], NULL, 0);
    long numAccounts = strtol(argv[2], NULL, 0);
    char* filename = argv[3];
    fOut = fopen(filename, "w");

    printf("%ld thread(s), %ld account(s), to %s\n", numThreads, numAccounts, filename);

    accounts = (account*) malloc(numAccounts*sizeof(account));
    initialize_accounts(numAccounts);

    // mutex for each account
    for(i = 0; i < numAccounts; i++) {
        pthread_mutex_init(&(accounts[i].mutex), NULL);
        accounts[i].amount = 0;
    }

    // create all threads with handler function
    pthread_t threads[numThreads];
    for(i = 0; i < numThreads; i++) {
        pthread_create(&threads[i], NULL, handleRequest, NULL);
    }

    //User input
    char* input = NULL;
    size_t len = 0;
    while(isRunning) {
        printf("> ");

        getline(&input, &len, stdin);
        strtok(input, "\n");

        if(strcmp(input, "END") == 0) {
            isRunning = 0;
            break;
        }

        // push command to queue
        pthread_mutex_lock(&queuetex);
        push(input, currentReqID);
        printf("< ID %d\n", currentReqID);
        pthread_mutex_unlock(&queuetex);
        currentReqID++;
    }

    // join all threads
    for(i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    /////////// PROGRAM END /////////////
    free_accounts();
    free(transactions);
    fclose(fOut);
    return;
}

void* handleRequest() {

    while(isRunning || transactions->head != NULL) {
        pthread_mutex_lock(&queuetex);

        if(transactions->head != NULL) {
            request req = pop();
            pthread_mutex_unlock(&queuetex);

            // pass into input handler to separate args
            char* type = strsep(&req.cmd, " ");
            char** args;
            int spaces = parseInput(req.cmd, args);

            // handle request logic
            if(strcmp(type, "CHECK") == 0) {
                int balance;
                account theAccount;
                int accID = atoi(args[1]);
                theAccount = accounts[accID - 1];

                pthread_mutex_lock(&theAccount.mutex);
                balance = read_account(accID);
                pthread_mutex_unlock(&theAccount.mutex);

                struct timeval done;
                gettimeofday(&done, NULL);

                flockfile(fOut);
                fprintf(fOut, "%ld.%06ld", done.tv_sec, done.tv_usec);
                funlockfile(fOut);

            } else if(strcmp(type, "TRANS") == 0) {
                int numTrans = spaces / 2;
                int accIDs[numTrans];
                int amounts[numTrans];
                int ISF = 0;

                int i;
                int accIndex = 0;
                int amountIndex = 0;

                // grab account # and amount separately
                for(i = 1; i <= spaces; i++) {
                    if(i % 2 == 0) {
                        amounts[amountIndex] = atoi(args[i]);
                        amountIndex++;
                    } else {
                        accIDs[accIndex] = atoi(args[i]);
                        accIndex++;
                    }
                }
                
                // flag ISF if there is a negative balance
                for(i = 0; i < numTrans; i++) {
                    int thisBalance = read_account(accIDs[i]);
                    if(thisBalance + amounts[i] < 0) {
                        ISF = 1;
                        break;
                    }
                }

                // don't write transactions if there was an ISF, otherwise do
                if(ISF) {
                    struct timeval done;
                    gettimeofday(&done, NULL);
                    flockfile(fOut);
                    fprintf(fOut, "%d ISF %d TIME %ld.%06ld %ld.%06ld\n", req.request_id, accIDs[i], req.starttime.tv_sec, req.starttime.tv_usec, done.tv_sec, done.tv_usec);
                    funlockfile(fOut);
                } else {
                    for(i = 0; i < numTrans; i++) {
                        account acc = accounts[accIDs[i]-1];
                        int thisBalance = read_account(accIDs[i]);
                        write_account(accIDs[i], (thisBalance+amounts[i]));
                    }
                    struct timeval done;
                    gettimeofday(&done, NULL);
                    flockfile(fOut);
                    fprintf(fOut, "%d OK TIME %ld.%06ld %ld.%06ld\n", req.request_id, req.starttime.tv_sec, req.starttime.tv_usec, done.tv_sec, done.tv_usec);
                    funlockfile(fOut);
                }

                // unlock all mutexes
                for(i = 0; i < numTrans; i++) {
                    account acc;
                    acc = accounts[accIDs[i]-1];
                    pthread_mutex_unlock(&acc.mutex);
                }
            } else { // else invalid input, do nothing
                pthread_mutex_unlock(&queuetex);
            }
        }
    }    
}

int parseInput(char* input, char* args[]) {
    int i, j;
    int spaces = 0;

    // get space count for use in account-amount pairs
    for(i = 0; i < strlen(input); i++) {
        if(input[i] == ' ') {
            spaces++;
        }
    }
    for(j = 0; j < 20; j++) { // max 10 accounts in transaction
        args[j] = strsep(&input, " ");
    }

    return spaces;
}

void startQueue() {
    transactions->head = NULL;
    transactions->tail = NULL;
    transactions->num_jobs = 0;
}

void push(char* cmd, int id) {
    request* newReq = malloc(sizeof(request));

    newReq->cmd = malloc(1024*sizeof(char));
    strncpy(newReq->cmd, cmd, 1024);
    newReq->request_id = id;
    gettimeofday(&(newReq->starttime), NULL);
    newReq->next = NULL;

    if(transactions->num_jobs > 0) {
        transactions->tail->next = newReq;
        transactions->num_jobs = transactions->num_jobs + 1;
    } else {
        transactions->head = newReq;
        transactions->tail = newReq;
    }
}

struct request pop() {
    request* temp;
    request req;

    if(transactions->num_jobs > 0) {
        req.request_id = transactions->head->request_id;
        req.starttime = transactions->head->starttime;
        req.cmd = malloc(1024*sizeof(char));
        strncpy(req.cmd, transactions->head->cmd, 1024);
        req.next = NULL;

        temp = transactions->head;
        transactions->head = transactions->head->next;
        free(temp->cmd);
        free(temp);

        if(!transactions->head) {
            transactions->tail = NULL;
        }

        transactions->num_jobs = transactions->num_jobs - 1;
    } else {
        req.cmd = NULL;
    }

    return req;
}