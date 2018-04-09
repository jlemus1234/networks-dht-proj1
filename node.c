#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include <semaphore.h>
#include <unistd.h>
//-----------------------------------
#define _GNU_SOURCE
#include <string.h>
#include <time.h> 
#include <sys/time.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//----------------------------------
#include "node.h"
#include "fileGen.h"
#include "hashing.h"
//-----------------------------------
// error - wrapper for perror
void error(char *msg) {
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	// Check for arguments
	int port = 9200;
	char *hostname = NULL;
	int  hostport = -1;
        if (argc != 2 && argc != 4) {
		fprintf(stderr, "usage: %s <port1> [IP address] [port2] \n", argv[0]); 
		fprintf(stderr, "where port1 is the port to run the program");
		fprintf(stderr, " on and port2 is the port number of a node ");
		fprintf(stderr, "on the network. IP Address is the IP of an ");
		fprintf(stderr, "existing node\n");
		exit(1);
        }

	if(argc == 2){
		// Establish listening port
                port = strtol(argv[1], NULL, 10);
        }else{
                hostname = argv[2];
                hostport = strtol(argv[3], NULL, 10);
        }

	// Generate pthread and split duties
        int initialized = FALSE;
	if(!initialized++){
		int success = pthread_mutex_init(&modTableState, NULL);
		if(success){
                        fprintf(stderr, "Failed to create mutex\n");
			exit(1);
                }
        }
	
	pthread_t inputThread;
	if(pthread_create(&inputThread, NULL, getInput, NULL) < 0){
		fprintf(stderr, "Failed to create thread \n");
        }

	network(port, hostname, hostport);
}

//pthread_mutex_lock(&modTableState);
//pthread_mutex_unlock(&modTableState);
//pthread_join(inputThread, NULL);

void network(int port, char *hostname, int hostport)
{
        // Try to open a socket
        int masterfd = socket(AF_INET, SOCK_STREAM, 0);         //0 uses default tcp, could try IPPROTO_TCP
        if(masterfd < 0) {                                      // Checks that succesfully opened socket
                fprintf(stderr, "Socket failed\n");
                exit(1);
        }
        fprintf(stderr, "opened a socket: %i\n", masterfd);

        // Set up a socket
        struct sockaddr_in serv_addr; 
        //const short PORT = 9200;
        const int BACKLOG = 0;
        memset(&serv_addr, 0, sizeof(serv_addr));       // set all bits to 0
        serv_addr.sin_family = AF_INET;                 
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(port);               // htons to make sure its in network byte order
                                                        // htons -- the s is for short
                                                        // htonl -- the l is for long (32 bit)

       fprintf(stderr, "set up a socket\n");
        // Close socket automatically when server closed
        int optval = 1;
        setsockopt(masterfd, SOL_SOCKET, SO_REUSEADDR,
             (const void *)&optval , sizeof(int));


        // Bind the socket
        int status = bind(masterfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
        if(status < 0) {
                fprintf(stderr, "Bind failed\n");
                exit(1);
        }
        fprintf(stderr, "tried to bind the socket: %i\n", status);

                                        // must maintain set of fds to track 
        fd_set master_fds;              // master fd list
        fd_set read_fds;                // temp fd used for select()

        FD_ZERO(&master_fds);           // zero out list of fds
        FD_ZERO(&read_fds);

        FD_SET(masterfd, &master_fds);  // add the listener/masterfd to the list
        int fdmax = masterfd;           // The maximum fd number -- equal to first socket at start

        listen(masterfd, BACKLOG);      // listen on socket fd with no backlog

        fprintf(stderr, "server iPv4 address: %s\n", inet_ntoa(serv_addr.sin_addr));
                //        clientiPv4 = inet_ntoa(client_addr.sin_addr);



        struct sockaddr_in client_addr; // Create temporary structure used to add clients
        unsigned int client_len = sizeof(client_addr);

	
	int z = 0; // Basic counter

	// Check if you have a node to introduce you into the network.
	// Establish a successor and predecessor
	
	/*if (argc == 4) {
		// Get table from peer
		char buf[1024];
		char joinReq[512];
                struct sockaddr_in destaddr; // dest addr //

		int portno = hostport;
		memcpy(joinReq, "Hello. This is a join request.", 512);
		destaddr.sin_family = AF_INET;
		struct hostent *network = gethostbyname(hostname);
		if (network == NULL) {
			fprintf(stderr, "ERROR, no such host as %s\n", network);
			exit(1);
                }
		bcopy((char *) network->h_addr,
                      (char *)&destaddr.sin_addr.s_addr, network->h_length);
		destaddr.sin_port = htons(portno);
		sendto(sockfd, &joinReq, sizeof(char) * 512, 0, (struct sockaddr *)&destaddr, sizeof(destaddr));
		recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL);
		printf("Reply from peer: %s of size %d\n", buf, n);
	}
	*/

	// Otherwise become the first node of your own network
	// ready to begin servicing requests
        for(;;){
		//fprintf(stderr, "In the loop %i\n", i);
		z++;
		sleep(1);
        }
        exit(1);
}

void* getInput()
{
	char str[100];
	//char *str = NULL;
	char t;
        for(;;){
		fflush(stdin);
		fprintf(stdout, "Enter a command:\nupload: ['u' filename] | "
				"search: ['s' filename.fh] | force ft update"
				"['t']\n");
		scanf(" %c %s", &t, (char *)&str);
		
		switch(t) {
                case 'u' :
                        fprintf(stderr, "Upload file:%s\n", str);
                        break;
                case 'd' :
                        fprintf(stderr, "Download file:%s\n", str);
			break;
		case 's' : 
			fprintf(stderr, "Search for file:%s\n", str);
			break;
		case 't' :
			fprintf(stderr, "Force update\n");
			//pthread_mutex_lock(&modTableState);

                        break;
                default :
			fprintf(stderr, "Invalid command type\n");
			//fprintf(stderr, "%c, %s\n", t, str);
                }

        }
	fprintf(stderr, "Broke outside of loop\n");
	exit(1);
}
