// data_race.c
#include <stdio.h>
#include <pthread.h>

static int counter = 0;  // shared, unsynchronized

void *worker(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        // BUG: unsynchronized read-modify-write
        counter++;
    }
    return NULL;
}

int main(void) {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, worker, NULL);
    pthread_create(&t2, NULL, worker, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    // Expected 2,000,000 if there were no data race
    printf("Final counter = %d (should be 2000000)\n", counter);
    return 0;
}
