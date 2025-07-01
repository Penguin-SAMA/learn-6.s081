#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define N 100000

int main() {
    int pipe1[2]; // parent -> child
    int pipe2[2]; // child -> parent
    char buf = 'A';

    struct timeval start, end;

    pipe(pipe1);
    pipe(pipe2);

    if (fork() == 0) {
        // child
        for (int i = 0; i < N; ++i) {
            read(pipe1[0], &buf, 1);
            write(pipe2[1], &buf, 1);
        }
        exit(0);
    } else {
        gettimeofday(&start, NULL);
        // parent
        for (int i = 0; i < N; ++i) {
            write(pipe1[1], &buf, 1);
            read(pipe2[0], &buf, 1);
        }
        gettimeofday(&end, NULL);

        long seconds = end.tv_sec - start.tv_sec;
        long useconds = end.tv_usec - start.tv_usec;
        double elapsed = seconds + useconds / 1000000.0;

        printf("Time taken for %d messages: %.6f seconds\n", N, elapsed);
        printf("Messages per second: %.2f\n", N / elapsed);
    }

    return 0;
}
