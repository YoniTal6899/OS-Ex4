#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "5555"

#define MAX_SIZE 100

void *get_in_addr(struct sockaddr *add)
{
    if (add->sa_family == AF_INET){return &(((struct sockaddr_in *)add)->sin_addr);}
    return &(((struct sockaddr_in6 *)add)->sin6_addr);
}
int main(int argc, char *argv[])
{
    int a_i_flag;
    int sock;
    struct addrinfo temp, *server_info, *curr;    
    char inet_buff[INET6_ADDRSTRLEN];
    if (argc != 2)
    {
        fprintf(stderr, "Error: enter hostname as an argument\n");
        exit(1);
    }
    memset(&temp, 0, sizeof(temp));
    temp.ai_family = AF_UNSPEC;
    temp.ai_socktype = SOCK_STREAM;
    if ((a_i_flag = getaddrinfo(argv[1], PORT, &temp, &server_info)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(a_i_flag));
        exit(1);
    }
    curr = server_info;

    while (curr != NULL)
    {
        if ((sock = socket(curr->ai_family, curr->ai_socktype,curr->ai_protocol)) == -1)
        {
            perror("ERROR creating socket! :(");
            curr = curr->ai_next;
            continue;
        }
        if (connect(sock, curr->ai_addr, curr->ai_addrlen) == -1)
        {            
            perror("ERROR connecting to server! :(");
            close(sock);
            curr = curr->ai_next;
            continue;
        }
        break;
    }
    if (!curr)
    {
        fprintf(stderr, "ERROR connecting to client! :(\n");
        exit(1);
    }
    inet_ntop(curr->ai_family, get_in_addr((struct sockaddr *)curr->ai_addr),inet_buff, sizeof(inet_buff));
    printf("Connecting to %s ...\n", inet_buff);
    freeaddrinfo(server_info);
    char buff[1024];
    int n = 0;
    int len;
    for(;;)
    {
        bzero(buff, sizeof(buff));
        n = 0;
        printf("Enter PUSH 'text', POP or TOP , to exit type EXIT\n");
        while ((buff[n++] = getchar()) != '\n'){}        
        buff[strlen(buff)-1] = '\0';
        
        if(strncmp(buff, "TOP", 3) == 0)
        {
            if (send(sock, buff, strlen(buff), 0) == -1)
            {
            perror("send");
            break;
            }
            if((len = recv(sock, buff, 1024, 0)) == -1)
            {
                perror("recv");
                exit(1);
            } 
            buff[len] = '\0';
            
            printf("%s\n", buff);
        }        
        else if((strncmp(buff, "EXIT", 4) == 0))
        {
            if (send(sock, buff, strlen(buff), 0) == -1)
            {
            perror("send");
            break;
            }             
            break;
        }
        else if((strncmp(buff, "PUSH", 4) == 0))
        {
            if (send(sock, buff, strlen(buff), 0) == -1)
            {
            perror("send");
            break;
            }
        }
        else if((strncmp(buff, "POP", 3) == 0))
        {
            if (send(sock, buff, strlen(buff), 0) == -1)
            {
                perror("send");                
                break;
            }
        }
        else{printf("Invalid command. Please enter valid command!\n");}        
    }
    close(sock);
    return 0;
}