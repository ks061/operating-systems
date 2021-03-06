#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 25
#define READ_END 0
#define WRITE_END 1

pid_t Fork(void);
int Pipe(int pipefd[2]);
int Read(int fd, void *buf, size_t count);
int Write(int fd, const void *buf, size_t count);

pid_t Fork(void) {
    int pid = fork();
    if (pid == -1) {
        perror("Error occurred when creating the child process.");
        exit(-1);
    } else {
        return pid;
    }
}

int Pipe(int pipefd[2]) {
    int pipe_creation_status = pipe(pipefd);
    if (pipe_creation_status < 0) {
        perror("An error occurred while creating the pipe.");
        exit(-1);
    } else {
        return pipe_creation_status;
    }
}

int Read(int fd, void *buf, size_t count) {
    int num_bytes_read = read(fd, buf, count);
    if (num_bytes_read < 0) {
        perror("An error occurred while reading the file.");
        exit(-1);
    } else {
        return num_bytes_read;
    }
}

int Write(int fd, const void *buf, size_t count) {
    int num_bytes_written = write(fd, buf, count);
    if (num_bytes_written < 0) {
        perror("An error occurred while writing the file.");
        exit(-1); 
    } else {
        return num_bytes_written;
    }
}

int main(void) {
    char write_msg[BUFFER_SIZE] = "Greetings";
    int fd[2];
    pid_t pid;

    char read_char;
    char write_char;

    /* create the pipe  */
    if (Pipe(fd) == -1) {
        fprintf(stderr, "Pipe failed");
        return 1;
    }

    /* fork a child process */
    pid = Fork();

    if (pid < 0) { /* error occured */
        fprintf(stderr, "Fork Failed");
        return 1;
    }

    if (pid > 0) { /* parent process */
        /* close the unused end of the pipe */
        close(fd[READ_END]);

        int write_shift = 0;
        write_char = write_msg[0];        

        /* write to the pipe one character at a time */
        while (write_char != 0) {
            Write(fd[WRITE_END], &write_char, 1);
            write_shift++;
            write_char = write_msg[write_shift]; 
        }

        /* close the write end of the pipe */
        close(fd[WRITE_END]);
    } else { /* child process */
        /* close the unused end of the pipe */
        close(fd[WRITE_END]);

        /* read from the pipe */
        while(Read(fd[READ_END], &read_char, 1) > 0) {
            printf("read: %c\n", read_char);
        }

        /* close the read end of the pipe */
        close(fd[READ_END]);
    }

    return 0;
}
