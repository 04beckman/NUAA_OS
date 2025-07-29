#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main() {
    char buf[128];
    while (1) {
        printf(1, "mysh17@ "); 
        memset(buf, 0, sizeof(buf));
        gets(buf, sizeof(buf)); //get input


        if (buf[0] == '\n' || buf[0] == '\0') {
            //printf(1, "\n"); ?????
            continue;
        }
        buf[strlen(buf)-1] = '\0'; 

        // 内置命令
        if (strcmp(buf, "quit") == 0) {
            exit();
        } else if (strcmp(buf, "promise") == 0) {
            printf(1, "I am ChenZipeng. I will study OS hard.\n");
        } 
        // 外部命令
        else if (strcmp(buf, "ls") == 0 || 
                 strcmp(buf, "sh") == 0 || 
                 strcmp(buf, "mysh17") == 0) {
            int pid = fork();
            if (pid == 0) {
                char *argv[] = { buf, 0 };
                exec(buf, argv);
                printf(1, "exec failed\n"); // exec失败时输出
                exit();
            } else {
                wait();
            }
        } 
        // 处理未知命令
        else {
            printf(1, "mysh17: command not found: %s\n", buf);
        }
    }
}
