#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(){
    const char* _pipe = "/tmp/myfifo";
    mkfifo(_pipe, 0666);
    const char* _pipe2 = "/tmp/myfifo2";
    mkfifo(_pipe2, 0666);
    const char* _pipe3 = "/tmp/myfifo3";
    mkfifo(_pipe3, 0666);

    printf("flag\n");


    int fd;
    pid_t pid = fork();
    if (pid == 0){   //first child
        fd = open(_pipe, O_RDONLY);
        if (fd == -1){
            perror("Child open() pipe1 failed\n");
            return 1;
        }

        int input;
        while(1){
            if(read(fd, &input, sizeof(int)) > 0) break;;
            //printf("counter\n");
        }
        printf("child1 read from parent: %d\n", input);

        close(fd);

        input *= input;

        printf("child1 modified input: %d\n", input);

        fd = open(_pipe2, O_WRONLY);
        if (fd == -1){
            perror("Parent open() failed\n");
            return 1;
        }

        write(fd, &input, sizeof(int));
        printf("child1 wrote to child2\n");
        
        close(fd);

        exit(0);
    } else {
        int input;
        printf("parent flag\n");
        printf("Input integer: ");
        scanf("%d", &input);
        
        fd = open(_pipe, O_WRONLY);
        if (fd == -1){
            perror("Parent open() failed\n");
            return 1;
        }

        write(fd, &input, sizeof(int));
        printf("parent wrote to child1\n");
        close(fd);

        waitpid(pid, NULL, 0);

        pid = fork();

        if (pid == 0){ //second child
            int input;

            fd = open(_pipe2, O_RDONLY || O_NONBLOCK);
            if (fd==-1){
                perror("failed child2 open()\n");
                return 1;
            }
            while(1){
                if(read(fd, &input, sizeof(int)) > 0) break;
                printf("child2 reading...\n");  
            }
            printf("second child read: %d\n", input);
            close(fd);

            input += 1;

            fd = open(_pipe3, O_WRONLY);
            write(fd, &input, sizeof(int));
            close(fd);

            exit(1);
        } else {
            int input;

            fd = open(_pipe3, O_RDONLY || O_NONBLOCK);    

            waitpid(pid, NULL, 0);

            while(1){
                printf("Parent waiting...\n");
                if(read(fd, &input, sizeof(int)) > 0) break;
            }

            printf("final int: %d\n", input);

        }
        
    }

    printf("END!!!\n");
    unlink("/tmp/myfifo");
    unlink("/tmp/myfifo2");
    unlink("/tmp/myfifo3");
    return 0;

}