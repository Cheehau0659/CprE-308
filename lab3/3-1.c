#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* thread1(void* ptr) {
    sleep(5);
    puts("Hello from thread 1");
}
void* thread2(void* ptr) {
    sleep(5);
    puts("Hello from thread 2");
}

void main() {
    pthread_t t1, t2;

    // These lines create each thread with the function
    pthread_create(&t1, NULL, &thread1, NULL); 
    pthread_create(&t2, NULL, &thread2, NULL);

    // These lines block until each thread terminates
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    puts("Hello from the main thread");
}