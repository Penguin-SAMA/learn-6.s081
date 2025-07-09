// 初始化进程

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/fs.h"
#include "kernel/file.h"
#include "user/user.h"
#include "kernel/fcntl.h"

char* argv[] = {"sh", 0};

int main(void) {
    int pid, wpid;

    // 初始化控制台
    if (open("console", O_RDWR) < 0) {
        mknod("console", CONSOLE, 0); // 创建控制台设备节点
        open("console", O_RDWR);      // 再次打开控制台
    }
    dup(0); // stdout
    dup(0); // stderr

    for (;;) {
        printf("init: starting sh\n");
        pid = fork();
        if (pid < 0) {
            printf("init: fork failed\n");
            exit(1);
        }
        if (pid == 0) {
            exec("sh", argv);
            printf("init: exec sh failed\n");
            exit(1);
        }

        for (;;) {
            // this call to wait() returns if the shell exits,
            // or if a parentless process exits.
            wpid = wait((int*)0);
            if (wpid == pid) {
                // shell退出，跳出循环，重新启动shell。
                break;
            } else if (wpid < 0) {
                printf("init: wait returned an error\n");
                exit(1);
            } else {
                // it was a parentless process; do nothing.
            }
        }
    }
}
