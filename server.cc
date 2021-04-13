#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <iostream>
#include <signal.h>

#define MAX_BUF 1024
#define MSG_LENGTH 100
int client_count = 0;

using namespace std;

//  type def structs for message passing scheme
typedef enum {
     REGULAR,
     EXIT,
     STATUS,
     TIME
} msg_type_t;

typedef struct msg {
  int client_pid;
}msg_initial_request_t;

typedef struct msg_r {
     msg_type_t type;
     char message_text[MSG_LENGTH];
}msg_client_request_t;

//  signal handler
void handle_sig(int sig){
  client_count--;
}

int main()
{
  /*    SERVER PROCESS    */
  int fd_server_IN, fd_client_recv, fd_client_send, msg_size;
  int fork_ret = 1;
  int clientpid;
  string tostring;
  char buf[MAX_BUF];
  char * server_IN_fifo = "./server_np";
  char client_send_fifo[100];
  char client_recv_fifo[100];
  string client_recv_fifo_s;
  string client_send_fifo_s;
  msg_initial_request_t msg_init;
  msg_client_request_t msg_s;

  //signal handling
  struct sigaction act;
  act.sa_handler = &handle_sig;
  sigaction(SIGCHLD, &act, 0);

  //time stuff init
  time_t rawtime;
  struct tm * ptm;;

    /* open, read, and display the message from the FIFO */
    mkfifo(server_IN_fifo, 0666);

    while(1)
    {
        fd_server_IN = open(server_IN_fifo, O_RDONLY);
        while(read(fd_server_IN, &msg_init, 104)>0)
        {
            printf("Received: %d\n", msg_init.client_pid);
            clientpid = msg_init.client_pid;
            fork_ret = fork();
            client_count++;

        }
        close(fd_server_IN);

        if(fork_ret == 0){
          while(1){
          tostring = to_string(clientpid);
          client_recv_fifo_s = "./" + tostring + "_recieve";
          client_send_fifo_s = "./" + tostring + "_send";
          strcpy(client_send_fifo, client_send_fifo_s.c_str());
          strcpy(client_recv_fifo, client_recv_fifo_s.c_str());

          mkfifo(client_send_fifo, 0666);
          mkfifo(client_recv_fifo, 0666);

          fd_client_send = open(client_send_fifo, O_RDONLY);
          while(read(fd_client_send, &msg_s, 104)>0){
            cout<<"message recieved"<<endl;
          }
          close(fd_client_send);

          if(msg_s.type == REGULAR){
            cout<<"message: "<<msg_s.message_text<<endl;
          }else{
            if(msg_s.type == EXIT){
              exit(EXIT_SUCCESS);
          }else{
            if(msg_s.type == STATUS){
              sprintf(buf, "%d", client_count);
              fd_client_recv = open(client_recv_fifo, O_WRONLY);
              if(write(fd_client_recv, buf, sizeof(buf)) < 0)
                cout<<"ERROR"<<endl;
              close(fd_client_recv);

            }else{
              if(msg_s.type == TIME){
                time (&rawtime);
                ptm = gmtime(&rawtime);
                int hrs, min;
                hrs = (ptm->tm_hour-4)%24;
                min = ptm->tm_min;
                string hrs_s = to_string(hrs);
                string min_s = to_string(min);
                string time_s = hrs_s + ":" + min_s;
                char time_str_c[time_s.length()];
                strcpy(time_str_c, time_s.c_str());
                cout<<endl<<"here's the time: "<<hrs_s<<":"<<min_s;
                sprintf(buf, "%s", time_str_c);
                fd_client_recv = open(client_recv_fifo, O_WRONLY);
                if(write(fd_client_recv, buf, sizeof(buf)) < 0)
                  cout<<"ERROR"<<endl;
                close(fd_client_recv);
              }
            }
            }
          }
        }//  end child while
      }// end child case

    }//  end of main while

    /* remove the FIFO */
    //unlink(client_IN_fifo);
    unlink(server_IN_fifo);
    unlink(client_send_fifo);
    unlink(client_recv_fifo);


    return 0;
}
