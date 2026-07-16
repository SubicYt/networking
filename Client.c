#include<winsock2.h>
#include <stdlib.h>
#include <stdio.h> 
#include <sys/types.h>
#include <ws2tcpip.h>

#pragma comment(lib, "win2_32.lib")

#define PORT_NUM "9090"
#define back_log 10
#define BRUH -1
#define MAX_DATA_SIZE 100

int main(int argc, char* argv[]){
    WORD wVersionRequired;
    WSADATA lpWsaData;
    int WSA_Status;
    
    wVersionRequired = MAKEWORD(2,2);
    
    WSA_Status = WSAStartup(wVersionRequired, &lpWsaData);
    if(WSA_Status == 0){
        printf("WSASTARTUP Success\n");
    }else{
        printf("WSASTARTUP FAILED: Error %d", WSA_Status);
        exit(EXIT_FAILURE);
    }

    ADDRINFO hints;
    memset(&hints, 0, sizeof(hints));
    ADDRINFO *results;
    ADDRINFO *hints_ptr;
    ADDRINFO *temp; //used to iterate through linkedlist;
    struct sockaddr_in *temp_cast_pointer;
    void* address;

    int getAddr_status;
    int connector_socket_fd;
    char address_to_connect_to_buffer[INET_ADDRSTRLEN];
    char message_buffer[MAX_DATA_SIZE];
    int bytes_rec;

    hints_ptr->ai_family = AF_INET;
    hints_ptr->ai_socktype = SOCK_STREAM;
    hints_ptr->ai_protocol = 0;

    getAddr_status = getaddrinfo(argv[1], PORT_NUM, &hints, &results);
    if(getAddr_status != 0){
        printf("getaddrinfo() failed: %d", getAddr_status);
        exit(EXIT_FAILURE);
    }
    else{
        printf("getaddrinfo() success\n");
    }
    
    for(temp = results; temp != NULL; temp = temp->ai_next){
        connector_socket_fd = socket(temp->ai_family, temp->ai_socktype, temp->ai_protocol);
        if(connector_socket_fd==-1){
            printf("socket() failed");
            exit(EXIT_FAILURE);
        }
        else{
            printf("socket success\n");
        }      
               
        if(connect(connector_socket_fd, temp->ai_addr, temp->ai_addrlen)==-1){
            printf("connect() failure");
            exit(EXIT_FAILURE);
        }else{
           break;
        }
    }

    temp_cast_pointer = (struct sockaddr_in *)temp->ai_addr;
    address = &(temp_cast_pointer->sin_addr);
    inet_ntop(temp->ai_family, address, address_to_connect_to_buffer, 
                sizeof(address_to_connect_to_buffer));
     printf("connected to: %s\n", address_to_connect_to_buffer);
    
    printf("here");
    bytes_rec = recv(connector_socket_fd, message_buffer, MAX_DATA_SIZE-1, 0);
    if(bytes_rec==-1){
        printf("Recieved message failure");
        exit(EXIT_FAILURE);
    }

    message_buffer[bytes_rec]='\0';
    printf("MESSAGE RECIEVED: %s", message_buffer);
    closesocket(connector_socket_fd);

    return 0;

}
