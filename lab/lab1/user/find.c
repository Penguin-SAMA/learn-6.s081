#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void find(char* path, char* file) {
    char buf[512], *p;
    int fd;
    struct dirent de; // Directory entry
    struct stat st;   // File status

    // 打开路径
    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // 获取文件状态
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    if (st.type == T_FILE) {
        char* name = path;
        for (char* s = path; *s; s++) {
            if (*s == '/') {
                name = s + 1;
            }
        }
        if (strcmp(name, file) == 0) {
            printf("%s\n", path);
        }
    }

    if (st.type == T_DIR) {
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
            printf("find: path too long\n");
            close(fd);
            return;
        }

        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';

        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0) {
                continue;
            }

            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
                continue; // Skip current and parent directory entries
            }

            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;

            if (stat(buf, &st) < 0) {
                printf("find: cannot stat %s\n", buf);
                continue;
            }

            find(buf, file);
        }
    }

    close(fd);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(2, "Usage: find <path> <filename>\n");
        exit(1);
    }

    find(argv[1], argv[2]);
    exit(0);
}
