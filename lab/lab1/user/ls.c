#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char* fmtname(char* path) {
    static char buf[DIRSIZ + 1];
    char* p;

    // Find first character after last slash.
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    // Return blank-padded name.
    if (strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
    return buf;
}

void ls(char* path) {
    char buf[512], *p;
    int fd;
    struct dirent de; // Directory entry
    struct stat st;   // File status

    // 打开文件
    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }

    // 获取文件状态
    if (fstat(fd, &st) < 0) {
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type) {
    // 设备文件或普通文件
    case T_DEVICE:
    case T_FILE:
        printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (int)st.size);
        break;

    // 目录
    case T_DIR:
        // 检查路径长度
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
            printf("ls: path too long\n");
            break;
        }
        // 拼接文件名
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        // 循环读取目录项
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            // de.inum == 0 代表空目录项
            if (de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0) {
                printf("ls: cannot stat %s\n", buf);
                continue;
            }
            printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (int)st.size);
        }
        break;
    }
    close(fd);
}

int main(int argc, char* argv[]) {
    int i;

    if (argc < 2) {
        ls(".");
        exit(0);
    }
    for (i = 1; i < argc; i++)
        ls(argv[i]);
    exit(0);
}
