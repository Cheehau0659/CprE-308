typedef struct account { // store a mutex for each account and the balance
    pthread_mutex_t mutex;
    int amount;
} account;

typedef struct request {
    struct request * next;    // pointer to the next request in the list
    char* cmd;                 // type of command
    int request_id;           // request ID assigned by the main thread
    struct timeval starttime; // starttime for TIME
} request;

typedef struct queue {
    struct request * head, * tail; // head and tail of the list
    int num_jobs;                  // number of jobs currently in queue
} queue;

void* handleRequest(); // do operations on accounts
int parseInput(char* input, char* args[]); // turn command stirng into argument array
void startQueue(); // initialize queue
void push(char* cmd, int id); // push to queue
struct request pop(); // pop from queue
