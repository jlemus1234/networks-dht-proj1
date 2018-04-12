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
#include <sys/ioctl.h>
#include <net/if.h>


// error - wrapper for perror
void error() {
	perror("ERROR:");
	exit(1);
}

dataArr *fdata; 


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

	// Create global table
	fdata = initdataArr();

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

void initNode (node* self, int myPort){
	int ipLen = 15;
	self -> port = myPort;
	self -> portSucc = myPort;
	self -> portPred = myPort;
	//self -> ipAdd = malloc (sizeof(char) * ipLen); // iPv4 -- 4 sets of 3 nums seperated by a period
	//memcpy(self->ipAdd, myIP, ip
	memset(self->ipAdd, '\0', ipLen);
	memset(self->ipSucc, '\0', ipLen);
	memset(self->ipPred, '\0', ipLen);
//memset(



}


void printNode (node* self){
        fprintf(stderr, "self: %s, %i\n", self->ipAdd ,self->port);
	fprintf(stderr, "succ: %s, %i\n", self->ipSucc, self->portSucc);
	fprintf(stderr, "pred: %s, %i\n", self->ipPred, self->portPred);
}

int send2(int port, char *hostname, int hostport){
  int sockfd, portno, n;
  fprintf(stderr, "\nin send\n");
    struct sockaddr_in serveraddr;
    struct hostent *server;
    //char *hostname;
    int BUFSIZE = 1024;
    char buf[BUFSIZE];

    hostname = hostname;
    portno = hostport;

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
	  (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);


    /* connect: create a connection with the server */
    if (connect(sockfd, &serveraddr, sizeof(serveraddr)) < 0) 
      error("ERROR connecting");

    struct ifreq ifr;

    ifr.ifr_addr.sa_family = AF_INET;

    snprintf(ifr.ifr_name, IFNAMSIZ, "ens192");

   ioctl(sockfd, SIOCGIFADDR, &ifr);

    printf("trying ip: %s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
    char ip [16];
    memset(ip, '\0', 16);
    //memcpy(self.ipAdd, ip, 16);


    snprintf(ip , 16 ,inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
    fprintf(stderr, "ip again: %s\n", ip);

    /* get message line from the user */
    /* printf("Please enter msg: ");
    bzero(buf, BUFSIZE);
    fgets(buf, BUFSIZE, stdin); */
    com joinreq;
    joinreq.type = htons(0);
    joinreq.stat = htons(1);
    memcpy(joinreq.sourceIP, ip, 16);
    joinreq.sourcePort = port;
    joinreq.length = 16;
    

    /* send the message line to the server */
    n = write(sockfd, (char *) &joinreq, sizeof(joinreq));
    if (n < 0) 
      error("ERROR writing to socket");

    /* print the server's reply */
    //bzero(buf, BUFSIZE);
    //n = read(sockfd, buf, BUFSIZE);
    //if (n < 0) 
    //  error("ERROR reading from socket");
    //printf("Echo from server: %s", buf);
    close(sockfd);
    return 0;



}


void network(int port, char *hostname, int hostport)
{
	node self;
	initNode(&self, port);
	int buffsize = 1024;
	char buff [buffsize];
	memset(buff, '\0', buffsize);

	//self.port = port;
	
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
		error();
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
	//strcpy (self.ipAdd, inet_ntoa(serv_addr.sin_addr));
    struct ifreq ifr;

    ifr.ifr_addr.sa_family = AF_INET;

    snprintf(ifr.ifr_name, IFNAMSIZ, "ens192");

   ioctl(masterfd, SIOCGIFADDR, &ifr);

   //printf("trying ip: %s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
    char ip [16];
    memset(ip, '\0', 16);
    snprintf(ip , 16 ,inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
    strcpy (self.ipAdd, ip);
    strcpy (self.ipSucc, ip);
    strcpy (self.ipSucc, ip);
    char *ipHash = hashNode(self.ipAdd, self.port, 0);
    //fprintf(stderr, "ip hash: %s\n", ipHash);
    strcpy (self.hash, ipHash);
    strcpy (self.hashSucc, ipHash);
    strcpy (self.hashSucc, ipHash);

	printNode(&self);
	
        struct sockaddr_in client_addr; // Create temporary structure used to add clients
        unsigned int client_len = sizeof(client_addr);
	
	int z = 0; // Basic counter

	// Check if you have a node to introduce you into the network.
	// Establish a successor and predecessor
	
        if (hostport > 0 && z == 0) {
                send2(port, hostname, port);
        }

	// Otherwise become the first node of your own network
	// ready to begin servicing requests
                
        for(;;){
		fprintf(stderr, "In for loop\n");
		//fprintf(stderr, "In the loop %i\n", i);
                if( z == 0){     z++;  }
		//sleep(1);
                read_fds = master_fds;
                if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1){
                        perror("select");
                        exit(1);
                }

                for(int i = 0; i <= fdmax; i++) {
                        if(FD_ISSET(i, &read_fds)) {
                                if (i == masterfd) {
                                        // immediately go into read mode here and do the checks for whether active
                                        // new client trying to connect probably
                                        fprintf(stderr, "new connection\n");
                                        int newClient;
                                        //size = sizeof(client_addr);
                                        newClient = accept(masterfd, (struct sockaddr *) &client_addr, &client_len);
                                        if(newClient < 0){
                                                perror("accept failed");
                                                continue;
                                                //exit(1);
                                        }else{
                                                fprintf(stderr,"accept succeeded -- fd: %i\n", newClient);
                                        }

                                        fprintf(stderr, "Server connected to new client serverside: ip %s, port %i\n", 
                                                inet_ntoa(client_addr.sin_addr), (int) ntohs(client_addr.sin_port));
                                        FD_SET(newClient, &master_fds); 
                                        if(fdmax < newClient){
                                                fdmax = newClient;
                                        }

                                        fprintf(stderr,"leaving adding new socket\n");
                                } else {
                                        fprintf(stderr, "activity found at %i\n", i);
                                        // handle data from a client
                                        //memset(buff, 0, sizeof(char)*450);
                                        memset(buff, '\0', buffsize);
                                        int nbytes;
                                        int sbytes = 0;
                                        com outCom;
                                        //message outMessage;
                                                // buff is undefined right now
                                        if((nbytes = recv(i, buff, sizeof(char) * buffsize, 0)) >=0) {
                                                if(nbytes == 0){
                                                        // deactivate clientID, remove from master_fd
                                                        fprintf(stderr, "connection closed\n");
                                                        FD_CLR(i, &master_fds);
                                                        // client closed the connection, can close the socket
                                                }else{
                                                        fprintf(stderr, "%i bytes received\n", nbytes);
                                                        //message* incMessage = (message*) buff;;
							com *incCom = (com*) buff;
                                                        //int type = ntohs(incMessage->type);;
							int type = ntohs (incCom->type);
							int stat = ntohs (incCom->stat);
							char sourceIP[15];
                                                        memcpy(sourceIP, incCom->sourceIP, 15);
							int sourcePort = ntohl(incCom->sourcePort);
							int length = ntohl(incCom->length);
							char reqHash[40];
                                                        memcpy(reqHash, incCom->reqHash, 40);
                                                        char data[512];
							memcpy(data, incCom->data, 512);

                                                        fprintf(stderr, "Finished copying\n");
                                                                //fprintf(stderr, "source: %s, destination: %s\n", incMessage->source, incMessage->destination);
                                                        fprintf(stderr, "source: %s, type: %i\n", sourceIP, type);
                                                        if(type == 0){
									// add the person in or pass along
                                                                if (status == 0) {
                                                                        fprintf(stderr, "status 0\n");
                                                                        if(strcmp (self.ipAdd, sourceIP) && (self.port == port)){
                                                                                fprintf(stderr, "Error: You can't join yourself\n");
                                                                                //  }else{strcmp (self.ipAdd, )

                                                                        }
                                                                        
                                                                                
                                                                }else if(status == 1){ // add yourself in now
                                                                        fprintf(stderr, "status 1\n");

                                                                }else{
                                                                        fprintf(stderr, "Incorrect status\n");
                                                                }
                                                        }else{
                                                                fprintf(stderr, "Invalid type\n");
                                                        }
                                                        /*   
                                                                        //////////////////////////////////////
                                                                        // Send hello ack
                                                                        //////////////////////////////////////
                                                                fprintf(stderr, "begin sending ack\n");

                                                                memcpy(outMessage.source, incDest, 20);
                                                                memcpy(outMessage.destination, incSource, 20);;

                                                                outMessage.type = htons(2);;
                                                                outMessage.length = htonl(0);;
                                                                outMessage.messageID = htonl(0);;
                                                                fprintf(stderr, "finished numbers\n");;

                                                                fprintf(stderr, "right before sending\n");;
                                                                if((sbytes = send(i, (char *)(&outMessage), (50), 0)) >= 0){
                                                                        fprintf(stderr, "acknowledge: sent %i bytes\n", sbytes);;
                                                                }else{
                                                                        fprintf(stderr, "error while writing\n");;
                                                                }
                                                                fprintf(stderr, "finished sending ack\n");;

                                                                        /////////////////////////////////////
                                                                        // Sending clientlist
                                                                        //////////////////////////////////////
                                                        
                                                                fprintf(stderr, "starting sending of clientList");;;
                                                                sbytes = 0;;;
                                                                clientList *list = getClientList(clientArray);;;
                                                                outMessage.type = htons(4);;
                                                                memcpy(outMessage.destination, incSource,20);;
                                                                memcpy(outMessage.source, "Server",20);;

                                                                outMessage.length = htonl(list->length);;
                                                                outMessage.messageID = htonl(0);;

                                                                if((sbytes = send(i, (char*)(&outMessage), 50, 0)) >= 0){
                                                                        fprintf(stderr, "clientlist header: sent %i bytes\n", sbytes);;
                                                                }else{
                                                                        fprintf(stderr, "error while writing\n");;
                                                                }
                                                                fprintf(stderr, "finished sending clientlist header\n");;
                                                                if((sbytes = send(i, (char *)(*list).list, list->length, 0)) >= 0){
                                                                        fprintf(stderr, "clientlist list: sent %i bytes\n", sbytes);;
                                                                }else{
                                                                        fprintf(stderr, "error while writing\n");
                                                                }
                                                                fprintf(stderr, "Finished sending actual list\n");;
                                                                        ///////////////////////////////////////
                                                                        */
                                                
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
	char str[100];
	//char *str = NULL;
	char t;
        for(;;){
		fflush(stdin);
		fprintf(stdout, "Enter a command:\nupload: ['u' filename] | download: ['d' filename] | "
				"search: ['s' filename.fh] | force finger table update"
				"['t'] | print file table ['p'] \n");
		scanf(" %c %s", &t, (char *)&str);
		
		switch(t) {
                case 'u' :
                        fprintf(stderr, "Upload file:%s\n", str);
			inputFile(fdata, &str[0]);
                        break;
                case 'd' :
                        fprintf(stderr, "Download file:%s\n", str);
			downloadFile(fdata, &str[0]);
			break;
		case 's' : 
			fprintf(stderr, "Search for file:%s\n", str);
			break;
		case 't' :
			fprintf(stderr, "Force update\n");
			//pthread_mutex_lock(&modTableState);
                        break;
		case 'p' :
			fprintf(stderr, "Print data table\n");
                        printDataArr (fdata);
			break;
                default :
			fprintf(stderr, "Invalid command type\n");
			//fprintf(stderr, "%c, %s\n", t, str);
                }

        }
	fprintf(stderr, "Broke outside of loop\n");
	exit(1);

}
