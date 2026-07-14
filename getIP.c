#include <stdlib.h>
#include <stdio.h> 
#include <sys/types.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib");

#define MAX_STR_SIZE 100
#define ipLen INET_ADDRSTRLEN

//in this case use localhost 127.0.0.1 for ipv4 and port 8080 to connect. 

//I guess good to remember is argc -> "argument count"
//argv-> "argument vector".


//IMPORTANT-> TO AVOID GETADDRINFO THROWING 10093 WE MUST PERFORM A SUCCESSFUL WSA STARTUP

int main(int argc, char* argv[]){
    //wsastartup
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 2);

    // Initialize Winsock
    int WSAresult = WSAStartup(wVersionRequested, &wsaData);
    if (WSAresult != 0) {
        printf( "WSAStartup failed: %d", WSAresult);
        exit(EXIT_FAILURE);
    }

    printf("Winsock initialized successfully.\n");


    if (argc != 2){
        printf("Usage Error, specify two cmdLine args");
        exit(EXIT_FAILURE);
    }

    //need to fill out struct addr_info
    /*
    int getaddrinfo(
        const char *node, // e.g. "www.example.com" or IP
        const char *service, // e.g. "http" or port number
        const struct addrinfo *hints,
        struct addrinfo **res
        );


        The hints argument points to an addrinfo structure that specifies
       criteria for selecting the socket address structures returned in
       the list pointed to by res.
       fill out getaddrinfo();
    */

    
    ADDRINFO hints; //---->use to fill out relavent info
    struct addrinfo *results; //---->linked list to store
    
    memset(&hints, 0, sizeof(hints));//clear out hints.

    ADDRINFO *addrinfo_ptr = &hints;
    addrinfo_ptr -> ai_family = AF_INET; //-> specify ipv4
    addrinfo_ptr -> ai_socktype = SOCK_STREAM; //STREAM SOCK -> TCP;
    addrinfo_ptr -> ai_flags = AI_PASSIVE; 
    //addrinfo_ptr -> ai_protocol = 0; //-> i think 0 for any? 
    
    int get_result = getaddrinfo(argv[1], NULL, &hints, &results);
    if(get_result ==0){
        printf("getaddrinfo() return %d: success\n", get_result);
    }else{
        printf("error, return: %d", get_result);
    }
    // res now points to a linked list of 1 or more
   // struct addrinfos

   ADDRINFO* temp; 
   for(temp = results; temp!=NULL; temp = temp->ai_next){
    char buffr[ipLen];
    struct sockaddr_in *ipv4_node;
    void *address;
    if(temp->ai_family == AF_INET){
        ipv4_node = (struct sockaddr_in *) temp->ai_addr;//casting this to a sockaddr_in
        address = &(ipv4_node -> sin_addr);
    }
    inet_ntop(temp->ai_family, address, buffr, sizeof(buffr));
    printf("IPV4 ADDRESS: %s\n", buffr);
   }
   freeaddrinfo(temp); //free linked list
   return 0;
}
