/*                                                                     
Client and server meant to just transmit bytes between each other.
What the server will do:

1.)getaddrinfo()

2.)loop through the servResults linked list and do the following:
        - create a socket and return a socket file descriptor
        - bind() the socket to some IP and Port number (PORT NUMBER IS HARD CODED)
        - We can now exit the loop and free the linked list

3.)we now are ready to listen()---> see parameters
    -The backlog argument defines the maximum length to which the queue
       of pending connections for sockfd may grow. Just an int.

4.)we can accept() a connection where the following must be noted:

        -accept() creates a new connected socket, and
            returns a new file descriptor referring to that socket.

        - The first argument is a socket that has been created with
       socket(2), bound to a local address with bind(2), and is listening
       for connections after a listen(2).

       - additionally struct sockaddr_storage. 
        This is where the information about the incoming connection will go (and
        with it you can determine which host is calling you from which port)

5.)Show which ip address/host you recieved a connection from

6.)send bytes to client
*/ 
#include<winsock2.h>
#include <stdlib.h>
#include <stdio.h> 
#include <sys/types.h>
#include <ws2tcpip.h>

#pragma comment(lib, "win2_32.lib")

#define PORT_NUM "9090"
#define back_log 10
#define BRUH -1

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
    ADDRINFO *results; //linked list for getaddrinfo()
    int getaddrinfo_status; // duh
    ADDRINFO *hints_pointer = &hints;

    ADDRINFO *temp; //used to loop through linkedlist
    int socket_fd; //socket file descriptor
    int new_id; //socket file descriptor returned by accept();

    struct sockaddr_storage client_addr;

    hints_pointer->ai_flags = AI_PASSIVE;
    hints_pointer->ai_family = AF_INET; //ipv4
    hints_pointer->ai_socktype = SOCK_STREAM;
    hints_pointer->ai_protocol = 0; //any protocol

    getaddrinfo_status = getaddrinfo(NULL, PORT_NUM, &hints, &results);
    if(getaddrinfo_status!=0){
        printf("getaddrinfo failed: returned: %d", getaddrinfo_status);
        exit(EXIT_FAILURE);
    }
    else{
        printf("getaddrinfo success\n");
    }

    for(temp = results; temp != NULL; temp = temp->ai_next){
        socket_fd = socket(temp->ai_family, temp->ai_socktype, temp->ai_protocol);
        if(socket_fd!=-1){
            printf("Socket created successfully with fd: %d\n", socket_fd);
        }else{
            printf("socket creation failed");
        }

        if(bind(socket_fd, temp->ai_addr, temp->ai_addrlen)!= -1){
            printf("bind success\n");
        }
        else{
            printf("bind failure");
            exit(EXIT_FAILURE);
        }
    }
    //done with this
    freeaddrinfo(results);
    if(listen(socket_fd, back_log)!=-1){
        printf("listening");
    }else{
        printf("failed to listen");
        exit(EXIT_FAILURE);
    }

    socklen_t sin_size = sizeof(client_addr);
    struct sockaddr_in *check_connection_addr;
    struct sockaddr *new_temp;
    void* address;
    char connected_address_buffer[100];

    new_id = accept(socket_fd, (struct sockaddr*)&client_addr, &sin_size);

    if(new_id==-1){
        printf("accept() failure:");
        exit(BRUH);
    }else{
        check_connection_addr = (struct sockaddr_in*)&client_addr;;
        address = &(check_connection_addr->sin_addr);
        inet_ntop(client_addr.ss_family, address, connected_address_buffer, sizeof(connected_address_buffer));
        printf("connected to: %s", connected_address_buffer);
    }
    
    if(send(new_id, "HELLO WORLD", 12, 0)==-1){
        printf("failure to send");
        exit(EXIT_FAILURE);
    }
    closesocket(new_id);
    return 0;
}
