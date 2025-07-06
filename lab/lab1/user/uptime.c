#include "kernel/types.h"
#include "user/user.h"

int main() {
    int ticks = uptime();
    printf("uptime: %d ticks\n", ticks);
    exit(0);
}
