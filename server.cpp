#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include "myStack.h"

#define PORT "5555"
#define BACKLOG 10

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void *myThread(void *vargp)
{   
    int len;
    char buff[1024];
    int sock_FD = *(int *)vargp;
    for (;;)
    {
        if((len = recv(sock_FD, buff, 1024, 0)) == -1)
        {
            perror("ERROR in recv func! :(\n");
            exit(1);
        } 

        buff[len] = '\0';

        if (strncmp(buff, "EXIT", 4) == 0)
        {
            printf("%d has exited!\n", sock_FD);
            break;
        }
        pthread_mutex_lock(&mutex);
                
        if (strncmp(buff, "TOP",3) == 0)
        {
            char *data = top();

            if(data == NULL){
                printf("Stack is empty!\n");
                if (send(sock_FD, "Stack is empty!", strlen("Stack is empty!"), 0) == -1){perror("send");}
                pthread_mutex_unlock(&mutex);
                continue;
            }
            char output[1024];
            sprintf(output, "OUTPUT: %s\n", data);
            printf("%s", output);
            if (send(sock_FD, output, strlen(output), 0) == -1){perror("send");}
        }
        else if (strncmp(buff, "PUSH", 4) == 0)
        {
            char *data = buff + 5;
            push(data);
        }
        else if (strncmp(buff, "POP", 3) == 0)
        {
            char *data = pop();
            if(data == NULL)
            {
                printf("Stack is empty!\n");
                pthread_mutex_unlock(&mutex);
                continue;
            }
        }
        else
        {
            printf("%d closed connection!\n", sock_FD);
            break;
        }
        pthread_mutex_unlock(&mutex);
    }
    close(sock_FD);
    return NULL;
}
void *get_IP_type(struct sockaddr *add)
{
    if (add->sa_family != AF_INET){return &(((struct sockaddr_in6 *)add)->sin6_addr);}
    return &(((struct sockaddr_in *)add)->sin_addr);    
}
void handle_sig(int harta_barta)
{
    int temp_err = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0){}
    errno = temp_err;
}



int main(void)
{
    int sock_FD, client_FD;
    struct addrinfo temp, *server_info, *curr;
    char inet_buff[INET6_ADDRSTRLEN];
    int a_i_flag;
    socklen_t sin_size;
    struct sigaction sig_act;    
    struct sockaddr_storage client_add;    

    memset(&temp, 0, sizeof(temp));
    temp.ai_flags = AI_PASSIVE;
    temp.ai_family = AF_UNSPEC;
    temp.ai_socktype = SOCK_STREAM;
    

    if ((a_i_flag = getaddrinfo(NULL, PORT, &temp, &server_info)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(a_i_flag));
         exit(1);
    }
    curr=server_info;
    while (curr != NULL)
    {
        if ((sock_FD = socket(curr->ai_family, curr->ai_socktype,curr->ai_protocol)) == -1)
        {
            perror("Error creating socket! :(\n");
            curr = curr->ai_next;
            continue;
        }
        int one = 1;
        if (setsockopt(sock_FD, SOL_SOCKET, SO_REUSEADDR,&one,sizeof(int)) == -1)
        {
            perror("Error in setsockopt! :(\n");
            curr = curr->ai_next;
            exit(1);
        }

        if (bind(sock_FD, curr->ai_addr, curr->ai_addrlen) == -1)
        {
            close(sock_FD);
            perror("Error in bind! :(\n");
            curr = curr->ai_next;
            continue;
        }
        curr = curr->ai_next;
        break;
    }
    freeaddrinfo(server_info);
    if (!curr)
    {
        fprintf(stderr, "Error in bind! :(\n");
        exit(1);
    }

    if (listen(sock_FD, BACKLOG) == -1)
    {
        perror("Error in listen! :(\n");
        exit(1);
    }

    sig_act.sa_handler = handle_sig;
    sigemptyset(&sig_act.sa_mask);
    sig_act.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sig_act, NULL) == -1)
    {
        perror("Error in sigaction! :(");
        exit(1);
    }
    printf("Server is up!\nReady for connections!\n");

    for(;;)
    {
        sin_size = sizeof(client_add);
        client_FD = accept(sock_FD, (struct sockaddr *)&client_add, &sin_size);
        if (client_FD == -1)
        {
            perror("Error in accept in loop! :(\n");
            continue;
        }
        inet_ntop(client_add.ss_family,get_IP_type((struct sockaddr *)&client_add),inet_buff, sizeof(inet_buff));
        printf("%s has connected to the server! :)\n", inet_buff);
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, myThread, &client_FD);
    }
    exit(0);
}