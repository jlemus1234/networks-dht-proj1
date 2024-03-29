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
#include "download.h"
//-----------------------------------
#include <sys/ioctl.h>
#include <net/if.h>

// error - wrapper for perror
void error() {
	perror("ERROR:");
	exit(1);
}

dataArr *fdata; 
DLQ* download;
int dip;
node self;

//pthread_mutex_lock(&modTableState);
//pthread_mutex_unlock(&modTableState);
//pthread_join(inputThread, NULL);

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
                port = strtol(argv[1], NULL, 10);
                hostname = argv[2];
                hostport = strtol(argv[3], NULL, 10);
        }

	// Create global table
	fdata = initdataArr();
        download = initDLQ();

	// Generate pthread and split duties
        int initialized = FALSE;
	if(!initialized++){
		int success = pthread_mutex_init(&modTableState, NULL);
		if(success){
                        fprintf(stderr, "Failed to create mutex\n");
			exit(1);
                }
        }

	// mutex for checking downloads
        initialized = FALSE;
	if(!initialized++){
		int success = pthread_mutex_init(&checkDL, NULL);
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

void printNode (node* self){
	fprintf(stderr, "\nPrinting node:\n");
        fprintf(stderr, "self: %s, %i\n%s\n", self->ipAdd , self->port,     self->hash);
        fprintf(stderr, "succ: %s, %i\n%s\n", self->ipSucc, self->portSucc, self->hashSucc);
        fprintf(stderr, "pred: %s, %i\n%s\n", self->ipPred, self->portPred, self->hashPred);
	fprintf(stderr, "\n");
}

void initNode (node* self, int myPort, int fd){
	int ipLen = 16;
	self -> port = myPort;
	self -> portSucc = myPort;
	self -> portPred = myPort;
	// iPv4 -- 4 sets of 3 nums seperated by a period
	memset(self->ipAdd, '\0', ipLen);
	memset(self->ipSucc, '\0', ipLen);
	memset(self->ipPred, '\0', ipLen);

	// Attempt to get the ip address
        struct ifreq ifr;
        ifr.ifr_addr.sa_family = AF_INET;
        // This is specific to the tufts machines running the comp112 servers
        snprintf(ifr.ifr_name, IFNAMSIZ, "ens192");
        ioctl(fd, SIOCGIFADDR, &ifr);

        char ip [16];
        memset(ip, '\0', 16);
        snprintf(ip , 16 ,inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
        strcpy (self->ipAdd, ip);
        strcpy (self->ipSucc, ip);
        strcpy (self->ipPred, ip);
        char *ipHash = hashNode(self->ipAdd, self->port, 0);
        strcpy (self->hash, ipHash);
        strcpy (self->hashSucc, ipHash);
        strcpy (self->hashPred, ipHash);
	printNode(self);
}

// Used to join an existing DHT when the node is first created
int joinDHT(int port, char *hostname, int hostport){
        int sockfd, portno, n;
        struct sockaddr_in serveraddr;
        struct hostent *server;

        hostname = hostname;
        portno = hostport;
        //  socket: create the socket 
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) 
                error("ERROR opening socket");

        // gethostbyname: get the server's DNS entry 
        server = gethostbyname(hostname);
        if (server == NULL) {
                fprintf(stderr,"ERROR, no such host as %s\n", hostname);
                exit(0);
        }

        // build the server's Internet address 
        bzero((char *) &serveraddr, sizeof(serveraddr));
        serveraddr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, 
              (char *)&serveraddr.sin_addr.s_addr, server->h_length);
        serveraddr.sin_port = htons(portno);


        // connect: create a connection with the server 
        if (connect(sockfd,(struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) 
                error("ERROR connecting");

        struct ifreq ifr;
        ifr.ifr_addr.sa_family = AF_INET;
        snprintf(ifr.ifr_name, IFNAMSIZ, "ens192");
        ioctl(sockfd, SIOCGIFADDR, &ifr);

        char ip [16];
        memset(ip, '\0', 16);
        snprintf(ip , 16 ,inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

        com joinreq;
        joinreq.type = htonl(0);
        joinreq.stat = htonl(0);
        memcpy(joinreq.sourceIP, ip, 16);
        joinreq.sourcePort = htonl(port);
        joinreq.length = htonl(16);
    

        // send the message line to the server 
        n = write(sockfd, (char *) &joinreq, sizeof(joinreq));
        if (n < 0) 
                error("ERROR writing to socket");

        close(sockfd);
        return 0;
}

// Pass data to another node
int pass(int length, char* data, char* hostname, int hostport){
        int sockfd, portno, n;
        //fprintf(stderr, "\nin pass\n");
        struct sockaddr_in serveraddr;
        struct hostent *server;

        hostname = hostname;
        portno = hostport;

        // socket: create the socket 
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) 
                error("ERROR opening socket");

        // gethostbyname: get the server's DNS entry 
        server = gethostbyname(hostname);
        if (server == NULL) {
                fprintf(stderr,"ERROR, no such host as %s\n", hostname);
                exit(0);
        }

        // build the server's Internet address 
        bzero((char *) &serveraddr, sizeof(serveraddr));
        serveraddr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, 
              (char *)&serveraddr.sin_addr.s_addr, server->h_length);
        serveraddr.sin_port = htons(portno);

        // connect: create a connection with the server 
        if (connect(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) 
                error("ERROR connecting");

        // send the message line to the server
        n = write(sockfd, data, length);
        if (n < 0) 
                error("ERROR writing to socket");

        close(sockfd);
        return 0;
}


void network(int port, char *hostname, int hostport)
{
	int buffsize = 1024;
	char buff [buffsize];
	memset(buff, '\0', buffsize);
	
        // Try to open a socket
        int masterfd = socket(AF_INET, SOCK_STREAM, 0);
        if(masterfd < 0) {                          // Checks that succesfully opened socket
                fprintf(stderr, "Socket failed\n");
                exit(1);
        }

        // Set up a socket
        struct sockaddr_in serv_addr; 
        //const short PORT = 9200;
        const int BACKLOG = 0;
        memset(&serv_addr, 0, sizeof(serv_addr));   // set all bits to 0
        serv_addr.sin_family = AF_INET;                 
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(port);          

        // Close socket automatically when server closed
        int optval = 1;
        setsockopt(masterfd, SOL_SOCKET, SO_REUSEADDR,
             (const void *)&optval , sizeof(int));

        // Bind the socket
        int status = bind(masterfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
        if(status < 0) {
                fprintf(stderr, "Bind failed\n");
		error();
                exit(1);
        }
                                        // must maintain set of fds to track 
        fd_set master_fds;              // master fd list
        fd_set read_fds;                // temp fd used for select()

        FD_ZERO(&master_fds);           // zero out list of fds
        FD_ZERO(&read_fds);

        FD_SET(masterfd, &master_fds);  // add the listener/masterfd to the list
        int fdmax = masterfd;           // The maximum fd number -- equal to first socket at start

        listen(masterfd, BACKLOG);      // listen on socket fd with no backlog

	initNode(&self, port, masterfd);

        struct sockaddr_in client_addr; // Create temporary structure used to add clients
        unsigned int client_len = sizeof(client_addr);
	
	int z = 0; // Basic counter

	// Check if you have a node to introduce you into the network.
	// Establish a successor and predecessor
        if (hostport > 0 && z == 0) {
                joinDHT(port, hostname, hostport);
        }

	// Otherwise become the first node of your own network
	// ready to begin servicing requests
        for(;;){
                if( z == 0){     z++;  }
                read_fds = master_fds;
                if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1){
                        perror("select");
                        exit(1);
                }

                for(int i = 0; i <= fdmax; i++) {
                        if(FD_ISSET(i, &read_fds)) {
                                if (i == masterfd) {
                                        // new client trying to connect 
                                        int newClient;
                                        newClient = accept(masterfd, (struct sockaddr *) &client_addr, &client_len);
                                        if(newClient < 0){
                                                perror("accept failed");
                                                continue;
                                                //exit(1);
                                        }
                                        FD_SET(newClient, &master_fds); 
                                        if(fdmax < newClient){
                                                fdmax = newClient;
                                        }
                                } else {
                                        // handle data from a client
                                        memset(buff, '\0', buffsize);
                                        int nbytes;
                                        com outCom;
                                        if((nbytes = recv(i, buff, sizeof(char) * buffsize, 0)) >=0) {
                                                if(nbytes == 0){
                                                        // client closed the connection, can close the socket
                                                        // deactivate clientID, remove from master_fd
                                                        FD_CLR(i, &master_fds);
                                                }else{
							com *incCom = (com*) buff;
							int type = ntohl (incCom->type);
							int stat = ntohl (incCom->stat);
							char sourceIP[16];
                                                        memcpy(sourceIP, incCom->sourceIP, 16);
							char IP2[16];
							memcpy(IP2, incCom->IP2, 16);
							int sourcePort = ntohl(incCom->sourcePort);
							int port2 = ntohl(incCom->port2);
							int length = ntohl(incCom->length);
							char reqHash[41];
                                                        memcpy(reqHash, incCom->reqHash, 41);
                                                        char data[512];
							memcpy(data, incCom->data, 512);

                                                        if(type == 0){
								// add the person in or pass along
                                                                if (stat == 0) {
                                                                        char *jhash = hashNode(sourceIP, sourcePort, 0);
									
                                                                        if(greaterThanHash (self.hash, self.hashSucc) == 2){
										// Initial join
                                                                                com joinreq;
                                                                                joinreq.type = htonl(0);
                                                                                joinreq.stat = htonl(1);
                                                                                memcpy(joinreq.sourceIP, self.ipAdd, 16);
                                                                                joinreq.sourcePort = htonl(self.port);
                                                                                memcpy(joinreq.IP2, self.ipAdd, 16);
                                                                                joinreq.port2 = htonl(self.port);
                                                                                joinreq.length = htonl(0);
                                                                                pass(sizeof(joinreq), (char*) &joinreq, sourceIP, sourcePort);

                                                                                memcpy(self.ipSucc, sourceIP, 16);
                                                                                self.portSucc = sourcePort;
										memcpy(self.hashSucc, jhash,41);
                                                                                memcpy(self.ipPred, sourceIP, 16);
                                                                                self.portPred = sourcePort;
                                                                                memcpy(self.hashPred, jhash, 41);
										printNode(&self);
                                                                                
                                                                        }else{
	
                                                                        if(greaterThanHash (self.hash, jhash) == 1){ // I am larger than jhash
                                                                                if(greaterThanHash(self.hashPred, jhash) == 1){ // my Pred is larger than jhash
											if(greaterThanHash(self.hashPred, self.hash) == 1) { // my pred is larger than me
												// Less than loop break case
												// to the new node
                                                                                                com joinreq;
                                                                                                joinreq.type = htonl(0);
                                                                                                joinreq.stat = htonl(1);
                                                                                                memcpy(joinreq.sourceIP, self.ipAdd, 16);
                                                                                                joinreq.sourcePort = htonl(self.port);
                                                                                                memcpy(joinreq.IP2, self.ipPred, 16);
                                                                                                joinreq.port2 = htonl(self.portPred);
                                                                                                joinreq.length = htonl(0);
                                                                                                pass(sizeof(joinreq), (char*) &joinreq, sourceIP, sourcePort);

												// to my predecessor
                                                                                                joinreq.type = htonl(0);
                                                                                                joinreq.stat = htonl(3);
                                                                                                memcpy(joinreq.sourceIP, sourceIP, 16);
                                                                                                joinreq.sourcePort = htonl(sourcePort);

                                                                                                joinreq.length = htonl(0);
                                                                                                pass(sizeof(joinreq), (char*) &joinreq, self.ipPred, self.portPred);

                                                                                                memcpy(self.ipPred, sourceIP, 16);
                                                                                                self.portPred = sourcePort;
                                                                                                memcpy(self.hashPred, jhash,41);
                                                                            
                                                                                                printNode(&self);
                                                                                        }else{
												// Passed to my predecessor for being too small
                                                                                                pass(nbytes, buff, self.ipPred, self.portPred);
                                                                                                printNode(&self);

                                                                                        }

                                                                                }else if(greaterThanHash(self.hashPred,jhash) == 0){ //jhash is larger than my pred
                                                                                        // insertion case: insert behind me --> send to my pred;
											// Passed to my predecessor to start join
                                                                                        pass(nbytes, buff, self.ipPred, self.portPred);
                                                                                        printNode(&self);

                                                                                }else{
                                                                                        fprintf(stderr, "This should never happen; ignore for now\n");
                                                                                        exit(1);
                                                                                }

                                                                        }else if(greaterThanHash (self.hash, jhash) == 0){ // jhash is larger than me
                                                                                if(greaterThanHash(self.hashSucc, jhash) == 1){ // jhash is smaller than my succ
                                                                                        // Insertion case: It is my successor
                                                                                        com joinreq;
                                                                                        joinreq.type = htonl(0);
                                                                                        joinreq.stat = htonl(1);
                                                                                        memcpy(joinreq.sourceIP, self.ipSucc, 16);
                                                                                        joinreq.sourcePort = htonl(self.portSucc);
											memcpy(joinreq.IP2, self.ipAdd, 16);
										        joinreq.port2 = htonl(self.port);
                                                                                        joinreq.length = htonl(0);

                                                                                        pass(sizeof(joinreq), (char*) &joinreq, sourceIP, sourcePort);
											// My successor's predecessor becomes it 
                                                                                        joinreq.type = htonl(0);
                                                                                        joinreq.stat = htonl(2);
											memcpy(joinreq.IP2, sourceIP, 16);
										        joinreq.port2 = htonl(sourcePort);
                                                                                        joinreq.length = htonl(0);
                                                                                        pass(sizeof(joinreq), (char*) &joinreq, self.ipSucc, self.portSucc);
											memcpy(self.hashSucc, jhash, 41); // it becomes my successor

											memcpy(self.ipSucc, incCom->sourceIP, 16);
                                                                                        self.portSucc = sourcePort;
                                                                                        printNode(&self);

                                                                                }else if(greaterThanHash(self.hashSucc,jhash) == 0){ // jhash is larger than my succ
											// Passed to my successor
                                                                                        if(greaterThanHash(self.hash, self.hashSucc) == 1){
												// greater loop break case
                                                                                                com joinreq;
                                                                                                joinreq.type = htonl(0);
                                                                                                joinreq.stat = htonl(1);
                                                                                                memcpy(joinreq.sourceIP, self.ipSucc, 16);
                                                                                                joinreq.sourcePort = htonl(self.portSucc);
                                                                                                memcpy(joinreq.IP2, self.ipAdd, 16);
                                                                                                joinreq.port2 = htonl(self.port);
                                                                                                joinreq.length = htonl(0);
                                                                                                pass(sizeof(joinreq), (char*) &joinreq, sourceIP, sourcePort);

												// to my successor
                                                                                                joinreq.type = htonl(0);
                                                                                                joinreq.stat = htonl(2);
                                                                                                memcpy(joinreq.IP2, sourceIP, 16);
                                                                                                joinreq.port2 = htonl(sourcePort);
                                                                                                joinreq.length = htonl(0);
                                                                                                pass(sizeof(joinreq), (char*) &joinreq, self.ipSucc, self.portSucc);

                                                                                                memcpy(self.ipSucc, sourceIP, 16);
                                                                                                self.portSucc = sourcePort;
                                                                                                memcpy(self.hashSucc, jhash,41);
                                                                                                printNode(&self);


                                                                                        }else{

                                                                                        pass(nbytes, buff, self.ipSucc, self.portSucc);
                                                                                        printNode(&self);

                                                                                        }
                                                                                }else{
                                                                                        fprintf(stderr, "This should never happen; ignore for now\n");
                                                                                        exit(1);

                                                                                }
                                                                        } else if(greaterThanHash(self.hash, jhash) == 2){
                                                                                fprintf(stderr, "This should never happen; ignore for now\n");
                                                                                exit(1);
                                                                        }

                                                                        }
                                                                        
                                                                }else if(stat == 1){ // add yourself in now
									// Update successor
                                                                        self.portSucc = sourcePort;
									memcpy(self.ipSucc, sourceIP, 16);
                                                                        char *jhash = hashNode(sourceIP, sourcePort, 0);
									memcpy(self.hashSucc, jhash, 41);
                                                                        free(jhash);
									// Update predecessor
									memcpy(self.ipPred , IP2, 16);
									self.portPred = port2;
									jhash = hashNode(IP2, port2, 0);
                                                                        memcpy(self.hashPred , jhash, 41);


									// Send request for all data
                                                                        outCom.type = htonl(0);
                                                                        outCom.stat = htonl(4);
                                                                        memcpy(outCom.sourceIP, self.ipAdd, 16);
                                                                        outCom.sourcePort = htonl(self.port);
                                                                        pass(sizeof(outCom), (char*) &outCom, sourceIP, sourcePort);
                                                                        printNode(&self);

                                                                }else if (stat == 2){
									// Large loop break 
									// Update only predecessor
                                                                        char *jhash = hashNode(IP2, port2, 0);
									memcpy(self.ipPred , IP2, 16);
									self.portPred = port2;
                                                                        memcpy(self.hashPred , jhash, 41);
                                                                        printNode(&self);

                                                                }else if(stat == 3){
									// Small loop break
									// Update only successor
                                                                        self.portSucc = sourcePort;
									memcpy(self.ipSucc, sourceIP, 16);
                                                                        char *jhash = hashNode(sourceIP, sourcePort, 0);
									memcpy(self.hashSucc, jhash, 41);
                                                                        free(jhash);
                                                                        printNode(&self);
                                                                }else if(stat == 4){
									// New node joined, send data
                                                                        joinDataSplit(fdata, &self);
                                                                }
                                                        }else if(type == 1){ // Data Request
                                                                char *rhash = hashNode(sourceIP, sourcePort, 0);

                                                                if(memcmp(rhash, self.hash, 41) == 0){
                                                                        fprintf(stderr, "Source equal to current node, stop sending\n");
                                                                }else{

								dataPair* data = getData(fdata, reqHash);
								if(data == NULL){
									// Pass request, don't have data
									pass(nbytes, buff, self.ipSucc, self.portSucc); // always passing to Succ
                                                                        printNode(&self);

                                                                }else{ // Found data, send it to requestor
                                                                        outCom.type = htonl(2);
                                                                        outCom.stat = htonl(3);
                                                                        memcpy(outCom.sourceIP, self.ipAdd, 16);
                                                                        outCom.sourcePort = htonl(self.port);
                                                                        outCom.length = htonl(data->len);
									memcpy(outCom.reqHash, reqHash, 41);
								        memcpy(outCom.data, data->data, 512);
                                                                        pass(sizeof(outCom), (char*) &outCom, sourceIP, sourcePort);
                                                                }
                                                                }
                                                                
                                                        }else if(type == 2){
								// Storing data
								dataPair new;
								new.key = &reqHash[0];
								new.data = &data[0];
								new.len = length;
                                                                insertPair(fdata, &new);
                                                                pthread_mutex_lock(&checkDL);

                                                                if(dip != 0){
                                                                        int finDL = checkDLQ(fdata, download);
                                                                        if(finDL == 1){
                                                                                writeDL(fdata, download);
                                                                                freeDLQ(download);
                                                                                initDLQ(download);
                                                                                dip = 0;
                                                                        }
                                                                }
                                                                pthread_mutex_unlock(&checkDL);

								// Pass the data along if necessary
								if(stat == 0){ // Passing to Predecessor if it is smaller
									if((greaterThanHash(reqHash, self.hashPred) == 1) || 
                                                                           (greaterThanHash(self.hashPred, self.hash) == 1)){
										// reqHash larger than pred, stop passing
                                                                        }else{
										// reqHash smaller than pred, keep passing
                                                                                pass(nbytes, buff, self.ipPred, self.portPred);
                                                                        }
                                                                }else if(stat == 1){ // Passing to successor
									if((greaterThanHash(reqHash, self.hashSucc) == 0) ||
                                                                           (greaterThanHash(self.hashSucc, self.hash) == 0)){
										// Stop passing to successor
                                                                        }else{
										// Passing to succesor
                                                                                pass(nbytes, buff, self.ipSucc, self.portSucc);
                                                                        }

                                                                }else{
                                                                        //fprintf(stderr, "Not passing data down\n");
                                                                
                                                                }
                                                        }else if (type == 3){
								//fprintf(stderr, "Received type 3 request\n");
								//fprintf(stderr, "Received %i bytes\n", nbytes);
								int dSize = 42;
								int type;
								char hash[41];
								char dataHash[41];
								memcpy(dataHash, buff + sizeof(int), 41);
                                                                char origHash[41];
                                                                memcpy(origHash, buff + sizeof(int) + 41, 41);

								char have = '1';
								int numNodes = (nbytes - sizeof(int)) / dSize;
                                                                //fprintf(stderr, "numNodes %i\n", numNodes);

								if(memcmp(origHash, self.hash, 41) == 0){
                                                                        //fprintf(stderr, "Back at the start\n");


								for (int i = 0; i < numNodes; i++){
                                                                        memcpy(&type, buff + 41 + (dSize * i), sizeof(int));
                                                                        memcpy(hash, buff + sizeof(int) + 41 + (dSize * i), 41);
                                                                        memcpy(&have, buff + sizeof(int) + 41 + 41 + (dSize * i), 1);
									fprintf(stderr, "                    | ^\n");
									fprintf(stderr, "                    v |\n");
									fprintf(stderr, "------------------------------------------\n");
									fprintf(stderr, "|%s|\n|                    %c                   |\n", hash, have);
									fprintf(stderr, "------------------------------------------\n");

                                                                }

                                                                }else{
                                                                        //fprintf(stderr, "Not at the start\n");
                                                                        char mess [nbytes + 42];
                                                                        dataPair* data = getData(fdata, dataHash);
                                                                        if(data == NULL){
                                                                                have = '0';
                                                                        }
									int newSize = nbytes + 42;
									
                                                                        memcpy(mess, buff, nbytes);
									memcpy(mess + nbytes, self.hash, 41);
									memcpy(mess + nbytes + 41, &have, 1); 
                                                                        pass(newSize, mess, self.ipSucc, self.portSucc);


                                                                }

                                                        }else{
                                                                fprintf(stderr, "Invalid type\n");

                                                        }
                                                }
                                        }
                                }

                        }
                }
        }
        exit(1);
}


void* getInput()
{
	dip = 0;
	char str[100]; // filenames must be under 100 characters
	char t;
        for(;;){
		fflush(stdin);
		fprintf(stdout, "Enter a command:\nupload: ['u' filename] | "
				"download: ['d' filename.fh] | "
				"scan: ['s' hash] | "
				"print chunk table ['p'] | \n"
                		"print node ['n'] | \n"
				"leave network: ['l' asd] |"
				"check current chunks: ['c' hash] |"
				"force finger table update ['t'] |"
                        );
		scanf(" %c %s", &t, (char *)&str);
		
		switch(t) {
                case 'u' :
                        fprintf(stdout, "Upload file:%s\n", str);
			inputFile(fdata, &str[0], &self);
                        break;
                case 'd' :
                        fprintf(stdout, "Download file:%s\n", str);
                        if(dip == 0){
                                fprintf(stdout, "Download begun: outputting to 'dlResult'\n");
                                beginDL(fdata, download, &str[0], &self);
                                dip = 1;
				pthread_mutex_lock(&checkDL);

                                int finDL = checkDLQ(fdata, download);
                                if (finDL == 1){
					dip = 0;
                                        writeDL(fdata, download);
                                        freeDLQ(download);
					initDLQ(download);
                                }
				pthread_mutex_unlock(&checkDL);

                        }else{
                                fprintf(stdout, "A download is already in progress\n");
                        }
			break;
		case 's' : 
			fprintf(stdout, "Scan Network:%s\n", str);
			char mess [(sizeof(int) + 41 + 42)];
			int type = htonl(3);
			memcpy(mess, &type, sizeof(type));
			memcpy(mess + sizeof(int), str, 41);
			memcpy(mess + sizeof(int) + 41, self.hash, 41);
			memcpy(mess + sizeof(int) + 41 + 41, "0", 1);
			
			pass(sizeof(int) + (sizeof(char) * 41) + (sizeof(char) * 42), mess, self.ipSucc, self.portSucc);
			break;
		case 'l' :
                        fprintf(stdout, "Leave network\n");
			leaveDataTransfer(fdata, &self);

			com lcom;
			// Send data to predecessor
                        lcom.type = htonl(0);
                        lcom.stat = htonl(3);
                        memcpy(lcom.sourceIP, self.ipSucc, 16);
                        lcom.sourcePort = htonl(self.portSucc);

                        lcom.length = htonl(0);
                        pass(sizeof(lcom), (char*) &lcom, self.ipPred, self.portPred);

			// Send update to successor
                        lcom.type = htonl(0);
                        lcom.stat = htonl(2);
                        memcpy(lcom.IP2, self.ipPred, 16);
                        lcom.port2 = htonl(self.portPred);
                        lcom.length = htonl(0);;
                        pass(sizeof(lcom), (char*) &lcom, self.ipSucc, self.portSucc);
                        exit(0);
       		case 't' :
			fprintf(stdout, "Force update\n");
			//pthread_mutex_lock(&modTableState);
                        break;
		case 'p' :
			fprintf(stdout, "Print data table\n");
                        printDataArr (fdata);
			break;
		case 'c' :
			fprintf(stdout, "Get element from data table\n");
			dataPair *temp = getData(fdata, str);
			if(temp == NULL){
                                fprintf(stderr, "Entry not found\n");
				fprintf(stderr, "used %zu, max %zu\n", fdata->used, fdata->max);
                        }else{
                                fprintf(stderr, "%s\n", temp->data);
                        }
                        break;
		case 'n' :
			printNode(&self);
                        break;
                default :
			fprintf(stderr, "Invalid command type\n");
                }

        }
	fprintf(stderr, "Broke outside of loop\n");
	exit(1);
}

