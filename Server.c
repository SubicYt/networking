#include <stdlib.h>
#include <stdio.h> 
#include <sys/types.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#define MYPORT "9000"
#define BACKLOG 10


#pragma comment(lib, "ws2_32.lib");

void *get_in_addr(struct sockaddr *sa){
 if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
 } //function stolen from beej lol. 


int main(int argc, char* argv[]){
    WSADATA wsaData; 
    WORD wVersionRequested;
    int error;

    wVersionRequested = MAKEWORD(2,2);
    error = WSAStartup(wVersionRequested, &wsaData);

    if(error !=0){
        printf("WSA STARTUP FAILED");
        exit(EXIT_FAILURE);
    }

    if(argc!=2){
        printf("Incorrect Usage");
        exit(EXIT_FAILURE);
    }

    /*
    Structure of Our server:
    Initialize socket,
    bind to port
    listen for traffic
    accept communications
    read the data stream from client 
    close the connection.
    */
    ADDRINFO hints;
    ADDRINFO *hints_pointer = &hints; //used to populate hints
    ADDRINFO* temp;//used in traversing results linked list
    ADDRINFO *results; //results linked list storing relavent data

    struct sockaddr_in *IPV4_NODE; //used in the for loop below to print out ip addresses. Again, might be temporary
    struct sockaddr_storage their_addr; // connector's address info

    socklen_t sin_size;
    char buffer[INET_ADDRSTRLEN]; //used to read out ipaddress (might be temporary);
    char str[INET6_ADDRSTRLEN];
    void* address; //used in the for loop below to print out ip addresses. Again, might be temporary
    int status; //used for getaddrinfo() status;

    int socket_fd, new_id;


    memset(&hints, 0, sizeof(hints));
    hints_pointer->ai_flags=AI_PASSIVE;
    hints_pointer->ai_family = AF_INET;
    hints_pointer->ai_socktype = SOCK_STREAM;
    hints_pointer->ai_protocol = 0; //0 for any.
    
    status = getaddrinfo(argv[1], MYPORT, &hints, &results);
    if(status ==0){

        printf("getaddrinfo() successful, returned: %d\n", status);//it worked 
        //now enter loop to read addresses
        for(temp = results; temp!=NULL; temp = temp->ai_next){

            if(temp->ai_family==AF_INET){//ensure ipv4, kind of like a confirmation;
                IPV4_NODE = (struct sockaddr_in *)temp->ai_addr;
                address = &(IPV4_NODE->sin_addr);

                inet_ntop(temp->ai_family, address, buffer, sizeof(buffer));
                printf("Using: %s\n", buffer);
            }else{
                printf("PLEASE KEEP USAGE TO IPV4 ADDRESS");
                exit(EXIT_FAILURE);
            }
            
        }
        freeaddrinfo(temp);
    }else{
        printf("failure at getaddrinfo(): error: %d", status);
        exit(EXIT_FAILURE);
    }
    
    for(temp = results; temp!=NULL; temp = temp->ai_next){
        socket_fd = socket(temp->ai_family, temp->ai_socktype, temp->ai_protocol);
        if(socket_fd==-1){
            printf("socket() failure error: %d", socket_fd);
            exit(EXIT_FAILURE);
        }else{
            printf("socket() successful, socket_fd: %d\n", socket_fd);
        }

        if(bind(socket_fd, temp->ai_addr, temp->ai_addrlen)==-1){
            printf("bind() failed.");
            closesocket(socket_fd);
            break;
        }else{
            printf("bind() successful\n");
        }
    }

    freeaddrinfo(results); //we no longer need this structure;

    if(listen(socket_fd, BACKLOG)==-1){
        printf("listen() failure");
        exit(EXIT_FAILURE);
    }else{
        printf("listen() Successful\n");
    }
    printf("Server is now awaiting connection...\n");
    
    while(1){//main accept() loop;
        sin_size = sizeof(their_addr);
        new_id = accept(socket_fd, (struct sockaddr *)&their_addr, &sin_size);
        if(new_id ==-1){
            printf("failure to accept");
            continue;
            exit(EXIT_FAILURE);
        }
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), 
        str, sizeof(str));
        printf("Recieved connection from %s \n", str);
    
        closesocket(new_id);
    }

    return 0;
}
