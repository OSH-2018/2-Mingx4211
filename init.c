#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

extern char **environ; //Environment Variable pointer

int main() {
    /* 输入的命令行 */
    char cmd[256];
    /* 命令行拆解成的各部分，以空指针结尾 */
    char *args[128];
    while (1) {
        /* 提示符 */
        printf("# ");
        fflush(stdin);
        fgets(cmd, 256, stdin);
        /* 清理结尾的换行符 */
        int i;
        for (i = 0; cmd[i] != '\n'; i++)
            ;
        cmd[i] = '\0';
        /* 拆解命令行 */
        args[0] = cmd;
        for (i = 0; *args[i]; i++)
            for (args[i+1] = args[i] + 1; *args[i+1]; args[i+1]++)
                if (*args[i+1] == ' ') {
                    *args[i+1] = '\0';
                    if (*(args[i+1]+1) != ' '){  //continuous spaces detection
                    args[i+1]++;
                    break;
                    }
                }
        args[i] = NULL;

        /* 没有输入命令 */
        if (!args[0]){
            printf("No Command Input!\n");
            continue;
        }

/*
        //pipe
        int count = 0, tmp = 0; //统计‘|’字符的数目
        int pos[10];  //记录“|”的位置
        for(tmp = 0; tmp < i; i++){
            if(strcmp(args[tmp],"|") == 0){
                pos[count] = tmp;
                count++;
            }
        }
        if(count == 1){
        int fd[2];
        char *args1[128], *args2[128];
        pid_t cpid;

        for(tmp = 0; tmp < pos[0]; tmp++)
            args1[tmp] = args[tmp];
        args1[tmp] = NULL;
        for(tmp = pos[0] + 1; tmp < i; tmp++)
            args2[tmp - pos[0] - 1] = args[tmp];
        args2[tmp - pos[0] - 1] = NULL;

        if(pipe(fd) == -1){
            perror("Pipe error");
        }

        cpid = fork();
        if(cpid == -1){
            perror("Fork error");
        }else if(cpid == 0){
            close(fd[0]);
            dup2(fd[1],STDOUT_FILENO);
            close(fd[1]);

            if(execvp(args1[0],args1) == 0)
                perror("First commad error");
        }
        else if(cpid > 0){
            close(fd[1]);
            dup2(fd[0],STDIN_FILENO);
            close(fd[0]);

            if(execvp(args2[0],args2) == 0)
                perror("Second Command Error");
        continue;
            
        }
        }else if(count >= 2 | count == 0){
            perror("Only simple pipe supported!");
        continue;
        }
        //

*/
        //Environment Variables
        //Command Env
        if(strcmp(args[0], "env") == 0) {
            char **env = environ;
            while (*env){
                puts(*env);
                env++;
            }
            continue;
        }
        //Command export
        if(strcmp(args[0], "export") == 0){
            if(!args[1])
               puts("Error:Too few args!");
            else if(strcmp(args[1], "-n") == 0 && args[2] != NULL){
                if(unsetenv(args[2]) != 0)
                perror("Unset environment variable failed!");
            }
            else{
                char name[128];
                char value[128];
                int j = 0, k = 0;
                int flag = 0;
                for(int i = 0; *(args[1] + i) != '\0'; i++){
                    if(*(args[1] + i) == '='){
                        flag = 1;
                        continue;
                    }
                    if(flag == 0){
                        name[j] = *(args[1] + i);
                        j++;
                    }
                    else if(flag == 1){
                        value[k] = *(args[1] + i);
                        k++;
                    }
                }
                name[j]='\0';
                value[k]='\0';
                if(name[0]==0||value[0]==0){
                    printf("No enough parameters!\n");
                }
                else{
                    if(setenv(name,value,1)!=0){
                        printf("%s\n",strerror(errno));
                    }
                }
            } 
            continue;
        }

        /* 内建命令 */
        //cd
        if (strcmp(args[0], "cd") == 0) {
            if (args[1]){
                if(chdir(args[1]) != 0)
                    perror("Chdir Failed!");
            }else
            perror("No Dictionary Inputed!");
            continue;
        }
        //pwd
        if (strcmp(args[0], "pwd") == 0) {
            char wd[4096];
            puts(getcwd(wd, 4096));
            continue;
        }
        //exit
        if (strcmp(args[0], "exit") == 0)
            return 0;


        /* 外部命令 */
        pid_t pid = fork();
        if (pid < 0){
            printf("%s\n",strerror(errno));
        }
        else if (pid == 0) {
            /* 子进程 */
            execvp(args[0], args);
            /* execvp失败 */
            printf("%s\n",strerror(errno));
            return 255;
        }
        /* 父进程 */
        wait(NULL);
    }
}