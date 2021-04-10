#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <iostream>

#define MAX_BUF 1024
#define MSG_LENGTH 100

using namespace std;

//  type def structs for message passing scheme
typedef enum {
     REGULAR,
     COMMAND
} msg_type_t;

typedef struct msg {
     msg_type_t type;
     char message_text[MSG_LENGTH];
}msg_t;


int main()
{
  int fd_server_IN, fd_client_IN;
  int fork_ret = 1;
  char buf[MAX_BUF];
  char * server_IN_fifo = "./server_IN_fifo";
  char * client_IN_fifo = "./client_IN_fifo";

    /* open, read, and display the message from the FIFO */

    while(1)
    {
        fd_server_IN = open(server_IN_fifo, O_RDONLY);

        if(fork_ret != 0)
        while(read(fd_server_IN, buf, MAX_BUF)>0)
        {
            printf("Received: %s\n", buf);
            fork_ret = fork();

        }

        if(fork_ret == 0)
        while(read(fd_server_IN, buf, MAX_BUF)>0)
        {
            printf("child server, Received: %s\n", buf);

        }


        close(fd_server_IN);
    }

    // fd_client_IN = open(client_IN_fifo, O_WRONLY);
    // sprintf(buf, "%d", getpid());
    // if( write(fd_client_IN, buf, sizeof(buf)) < 0)
    //     fprintf(stderr, "Error\n");
    // close(fd_client_IN);


    /* remove the FIFO */
    unlink(client_IN_fifo);
    unlink(server_IN_fifo);

    return 0;
}
