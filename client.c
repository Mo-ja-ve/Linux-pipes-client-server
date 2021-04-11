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

//written by Andriy Kalinichenko
int main()
{
    /*    CLIENT proccess    */

    //file discriptors
    int fd_client_send, fd_client_recv, ret, msg_size;
    char buf[MAX_BUF];
    int clientpid = getpid();
    string tostring = to_string(clientpid);
    //string used to take user request
    string client_str;
    //strings used to name FIFO which will be created
    string server_IN_fifo_s = "./" + tostring + "_send";
    string client_IN_fifo_s = "./" + tostring + "_recieve";
    char * server_IN_fifo = "./server_np";
    //string to char conversion
    char client_send_fifo[100];
    char client_recieve_fifo[100];
    //char arrays to check if inputs are valid, will be tested against later
    char c_send[6] = {'s','e','n','d',':','\0'};
    char c_exit[5] = {'e','x','i','t','\0'};
    strcpy(client_send_fifo,server_IN_fifo_s.c_str());
    strcpy(client_recieve_fifo,client_IN_fifo_s.c_str());
    msg_t msg;
    msg_type_t typ;

    char client_msg[MSG_LENGTH];

    /* create the FIFO (named pipe) */
    //mkfifo(client_send_fifo, 0666);
    //mkfifo(client_recieve_fifo, 0666);

    /* write PID to the FIFO */
    fd_client_send = open(server_IN_fifo, O_WRONLY);
    sprintf(buf, "%d", getpid());
    if( write(fd_client_send, buf, sizeof(buf)) < 0)
        fprintf(stderr, "Error\n");

    // we send additional requests to the server
    cout<<endl<<"hello, type server request: ";
    cin >> client_str;
    int n = client_str.length();
    char temp_s[n+1];
    strcpy(temp_s, client_str.c_str());
    temp_s[5] = '\0';

    //checking to see if it's a send case
    if(temp_s[0] == c_send[0] && temp_s[1] == c_send[1] && temp_s[2] == c_send[2] &&
       temp_s[3] == c_send[3] && temp_s[4] == c_send[4] && temp_s[5] == c_send[5]){

         //below we remove the send: portion of the string and send it to server, also set enum to REGULAR
         int c_string_length = client_str.length();
         typ = REGULAR;
         msg.type = typ;
         client_str.erase(0,5);
         strcpy(temp_s, client_str.c_str());
         strcpy(msg.message_text, temp_s);

         fd_client_send = open(client_send_fifo, O_WRONLY);

         msg_size = sizeof(msg);
         
         if( write(fd_client_send, &msg_size, sizeof(msg)) < 0);
            fprintf(stderr, "Error\n");

         if( write(fd_client_send, &msg, sizeof(msg)) < 0);
            fprintf(stderr, "Error\n");

    }else{// checking to see if it's an exit case
        if(temp_s[0] == c_exit[0] && temp_s[1] == c_exit[1] && temp_s[2] == c_exit[2] &&
           temp_s[3] == c_exit[3] && temp_s[4] == c_exit[4]){

          //exit command, we set enum and send it to server
           typ = COMMAND;
           msg.type = typ;
           if( write(fd_client_send, &msg, sizeof(msg)) < 0);
               fprintf(stderr, "Error\n");

           exit(EXIT_SUCCESS);
       }else
         cout<<endl<<"WARNING: INVALID INPUT";
     }

    // strcpy(buf, client_str.c_str());
    // if( write(fd_server_IN, buf, sizeof(buf)) < 0)
    //     fprintf(stderr, "Error\n");
    //
    //
    //
    // close(fd_server_IN);
    //
    //
    //
    // while(1)
    // {
    //     fd_client_IN = open(client_IN_fifo, O_RDONLY);
    //
    //     while(read(fd_client_IN, buf, MAX_BUF)>0)
    //     {
    //         printf("Received: %s\n", buf);
    //
    //     }
    //
    //     close(fd_client_IN);
    // }

    return 0;
}
