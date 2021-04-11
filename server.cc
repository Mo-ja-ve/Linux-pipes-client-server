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
  int fd_server_IN, fd_client_recv, fd_client_send, msg_size;;
  int fork_ret = 1;
  int clientpid;
  string tostring;
  char buf[MAX_BUF];
  char * server_IN_fifo = "./server_np";
  char client_send_fifo[100];
  char client_recieve_fifo[100];
  string client_recv_fifo_s;
  string client_send_fifo_s;
  msg_t msg_s;

    /* open, read, and display the message from the FIFO */
    mkfifo(server_IN_fifo, 0666);

    while(1)
    {
        fd_server_IN = open(server_IN_fifo, O_RDONLY);
        while(read(fd_server_IN, buf, MAX_BUF)>0)
        {
            printf("Received: %s\n", buf);
            clientpid = atoi(buf);
            fork_ret = fork();
        }
        close(fd_server_IN);

        if(fork_ret == 0){
          while(1){
          tostring = to_string(clientpid);
          client_recv_fifo_s = "./" + tostring + "_recieve";
          client_send_fifo_s = "./" + tostring + "_send";
          strcpy(client_send_fifo, client_send_fifo_s.c_str());
          mkfifo(client_send_fifo, 0666);

          fd_client_send = open(client_send_fifo, O_RDONLY);

          while(read(fd_client_send, &msg_s, 104)>0){
            cout<<"message recieved"<<endl;
          }
          close(fd_client_send);

          cout<<"message "<<msg_s.message_text<<endl;
          cout<<"type: "<<msg_s.type<<endl;
        }
      }
    }

    // fd_client_IN = open(client_IN_fifo, O_WRONLY);
    // sprintf(buf, "%d", getpid());
    // if( write(fd_client_IN, buf, sizeof(buf)) < 0)
    //     fprintf(stderr, "Error\n");
    // close(fd_client_IN);


    /* remove the FIFO */
    //unlink(client_IN_fifo);
    unlink(server_IN_fifo);

    return 0;
}
