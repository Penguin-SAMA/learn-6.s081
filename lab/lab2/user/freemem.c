
#include "kernel/types.h"
#include "user/user.h"

int main() {
    int freebytes = freemem();
    printf("Free memory: %d bytes (%d KB)\n", freebytes, freebytes / 1024);
    exit(0);
}
