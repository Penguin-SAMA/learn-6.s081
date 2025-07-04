#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAXLINE 512

int main(int argc, char* argv[]) {
    char buf[MAXLINE];
    int n = 0;
    char ch;

    while (1) {
        n = 0;
        // 先读一行
        while (read(0, &ch, 1) == 1) {
            if (ch == '\n') {
                buf[n] = 0;
                break;
            }

            buf[n++] = ch;
            if (n >= MAXLINE - 1) {
                break;
            }
        }

        if (n == 0) {
            break;
        }

        // 构造参数数组
        char* exec_argv[MAXARG];
        int i;

        for (i = 1; i < argc; ++i) {
            exec_argv[i - 1] = argv[i];
        }
        int arg_base = i - 1;

        // 分词处理 buf，把单词存入exec_argv[]
        char* p = buf;
        while (*p != '\0') {
            while (*p == ' ') {
                p++;
            }

            if (*p == '\0') {
                break;
            }

            // 添加参数
            exec_argv[arg_base++] = p;

            while (*p != '\0' && *p != ' ') {
                p++;
            }
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
        }

        exec_argv[arg_base] = 0; // 结尾添加NULL

        // 执行命令
        if (fork() == 0) {
            exec(exec_argv[0], exec_argv);
            fprintf(2, "xargs: exec %s failed\n", exec_argv[0]);
            exit(1);
        }
        wait(0);
    }
    exit(0);
}
