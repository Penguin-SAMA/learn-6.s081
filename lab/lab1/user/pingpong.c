#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
    int pipe1[2]; // parent -> child
    int pipe2[2]; // child -> parent
    char buf[1];

    pipe(pipe1); // Create pipe for parent to child communication
    pipe(pipe2); // Create pipe for child to parent communication

    int pid = fork();

    if (pid < 0) {
        fprintf(2, "fork failed\n");
        exit(1);
    }

    if (pid == 0) {
        read(pipe1[0], buf, 1); // Wait for "ping" from parent
        printf("%d: received ping\n", getpid());

        write(pipe2[1], "p", 1); // Send "pong" to parent
        exit(0);
    } else {
        write(pipe1[1], "p", 1); // Send "ping" to child
        wait(0);

        read(pipe2[0], buf, 1); // Wait for "pong" from child
        printf("%d: received pong\n", getpid());

        exit(0);
    }
}
