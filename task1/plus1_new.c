#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void checkfdnull(int fd){
    if (fd == NULL){
        perror("fd null");
        exit(1);
    }
}

int main(){
    const char* _pipe_parent_1 = "/tmp/myfifo";
    const char* _pipe_1_2 = "/tmp/myfifo2";
    const char* _pipe_2_parent = "/tmp/myfifo3";
    
    mkfifo(_pipe_parent_1, 0666);
    mkfifo(_pipe_1_2, 0666);
    mkfifo(_pipe_2_parent, 0666);

    pid_t child1_pid = fork();
    pid_t child2_pid;
    if (child1_pid == 0){
        //child1 read from parent
        int _fd_parent_1 = open(_pipe_parent_1, O_RDONLY);
        checkfdnull(_fd_parent_1);

        int input;
        while(1){
            if (read(_fd_parent_1, &input, sizeof(int)) > 0) break;
        }

        printf("child1 read: %d\n", input);
        input *= input;
        close(_fd_parent_1);
        //

        printf("flag:");
        
        //child1 write to child2
        int _fd_1_2 = open(_pipe_1_2, O_WRONLY );
        printf("Flag: %d\n", _fd_1_2);
        checkfdnull(_fd_1_2);

        write(_fd_1_2, &input, sizeof(int));
        printf("child 1 wrote: %d\n", input);
        close(_fd_1_2);

        exit(0);
    } else {
        int _fd_parent_1 = open(_pipe_parent_1, O_WRONLY);
        checkfdnull(_fd_parent_1);
        
        int input;
        printf("Input integer: ");
        scanf("%d", &input);
        printf("\n");
        
        write(_fd_parent_1, &input, sizeof(int));
        close(_fd_parent_1);
        input = NULL;

        pid_t child2_pid = fork();
        if (child2_pid == 0){
            //child2 read from child1
            int _fd_1_2 = open(_pipe_1_2, O_RDONLY);
            checkfdnull(_fd_1_2);

            while(1){
                if (read(_fd_1_2, &input, sizeof(int))) break;
            }
            printf("child 2 read: %d\n", input);

            input += 1;
            close(_fd_1_2);
        
            //child2 write to parent
            int _fd_2_parent = open(_pipe_2_parent, O_WRONLY);
            checkfdnull(_fd_2_parent);

            write(_fd_2_parent, &input, sizeof(int));
            printf("child 2 wrote: %d\n", input);
            close(_fd_2_parent);

            exit(0);
        } else {
            //parent read from child2
            int _fd_2_parent = open(_pipe_2_parent, O_RDONLY);
            checkfdnull(_fd_2_parent);

            while (1){
                if (read(_fd_2_parent, &input, sizeof(int))) break;
            }

            close(_fd_2_parent);

            printf("Final int: %d\n", input);
        }


        //process second child stuff
    }
    wait(child1_pid, NULL, 0);
    wait(child2_pid, NULL, 0);

    printf("ALL DONE\n");
    unlink("/tmp/myfifo");
    unlink("/tmp/myfifo2");
    unlink("/tmp/myfifo3");
    exit(0);
}