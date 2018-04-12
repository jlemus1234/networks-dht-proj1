/*
 * node.c - A simple UDP echo server
 * usage: udpserver <port>
 */
 
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/select.h>

#define BUFSIZE 1024
#define NUM_KEYS 6
#define KEY_SIZE 20
#define IP_ADDR_SIZE 20

typedef struct __attribute__ ((__packed__)) table_key{
	char key[KEY_SIZE];
	char ip[IP_ADDR_SIZE];
}table_key;

char lookup_table[NUM_KEYS][KEY_SIZE + IP_ADDR_SIZE];	
char predecessor[KEY_SIZE + IP_ADDR_SIZE];
char self[KEY_SIZE + IP_ADDR_SIZE];

int inputAvailable();
void print_lookup();
int join(char buf[], struct sockaddr_in clientaddr, int sockfd);
int down(char buf[], struct sockaddr_in clientaddr, int sockfd);
int update_table(char buf[], struct sockaddr_in clientaddr, int sockfd);
int update_pred(struct sockaddr_in clientaddr);
int join_pred(int sockfd);

/*
 * error - wrapper for perror
 */
void error(char *msg) {
	perror(msg);
	exit(1);
}

typedef struct __attribute__((__packed__)) join_req{
	char type;
	char key[KEY_SIZE]; /* sending - your key; receiving - their key */
}join_req; 

typedef struct __attribute__((__packed__)) join_resp{
	char type;
	char key[KEY_SIZE]; /*predecessor: sending yourself; receiving:  */
	table_key tk_in; /* successor - this is your successor */
}join_resp;  

typedef struct __attribute__((__packed__)) down_req{
	char type;
	char key[KEY_SIZE];
}down_req; 

typedef struct __attribute__((__packed__)) down_resp{
	char type;
	char data[512];
}down_resp; 

typedef struct __attribute__((__packed__)) pred_resp{
	char type; /*You are sending your own key */
	
}pred_resp; 

int main(int argc, char **argv) {
	int sockfd; /* socket */
	int portno; /* port to listen on */
	int clientlen; /* byte size of client's address */
	struct sockaddr_in serveraddr; /* server's addr */
	struct sockaddr_in destaddr; /* dest addr */
	struct sockaddr_in clientaddr; /* client addr */
	struct hostent *network; /* client host info */
	char buf[BUFSIZE]; /* message buf */
	char *hostaddrp; /* dotted decimal host addr string */
	int optval; /* flag value for setsockopt */
	int n; /* message byte size */
	char fname[20];

	/* 
         * check command line arguments
         */
	if (argc != 2 && argc != 4) {
		fprintf(stderr, "usage: %s <port1> [IP address] [port2] \n", argv[0]); 
		fprintf(stderr, "where port1 is the port to run the program");
		fprintf(stderr, " on and port2 is the port number of a node ");
		fprintf(stderr, "on the network. IP Address is the IP of an ");
		fprintf(stderr, "existing node\n");
		exit(1);
        }
	portno = atoi(argv[1]);

	/*
         * socket: create the parent socket
         */ 
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");

	/*
         * setsockopt: Handy debugging trick that lets
         * us rerun the server immediately after we kill it;
         * otherwise we have to wait about 20 secs.
         * Eliminates "ERROR on binding: Address already in use" error.
         */
	optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                   (const void *)&optval, sizeof(int));


	/*
         *	setsockopt for timeout
         */
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 1000;
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
		error("Error");
	
	/*
         * build the server's Internet address
         */	
	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);	 
	serveraddr.sin_port = htons((unsigned short)portno);

	/*
         * bind: associate the parent socket with a port
         */
	if (bind(sockfd, (struct sockaddr *) &serveraddr, 
		sizeof(serveraddr)) < 0)
		error("ERROR on binding");


	/***********************************************************************
         *								       *
         *	At this point, the peer is set up as a server. Now, it 	       *
         *	will look for another node so that it can join the network.    *
	 *								       *
         **********************************************************************/

	
	
	/* JOIN REQUEST */	
	if (argc == 4) {
		/* Create Join Request*/
		char buf[1024];
		join_req req;
		req.type = 1;
	       	char mess[20] = "HASH KEY FROM PEER"; /* should be your hash key */
//		fprintf(stderr, "mess: %s\n", mess);
		memcpy(&req.key, mess, KEY_SIZE);

//		fprintf(stderr, "Here\n");
		/* Build Dest */
		int portno = atoi(argv[3]);
		destaddr.sin_family = AF_INET;
		network = gethostbyname(argv[2]);
		fprintf(stderr, "Sending Join Request to Peer:\nmessage:%c\nkey:%s\n\n", req.type, req.key);
		if (network == NULL) {
			fprintf(stderr, "ERROR, no such host as %s\n", network);
			exit(0);
                }
		bcopy((char *) network->h_addr,
                      (char *)&destaddr.sin_addr.s_addr, network->h_length);
		destaddr.sin_port = htons(portno);
		//fprintf(stderr, "Here bish\n");

		/* sending message */	
		sendto(sockfd, &req, sizeof(join_req), 0, (struct sockaddr *)&destaddr, sizeof(destaddr));
	}

//        else{        /*set up table by yourself*/ 
		fprintf(stderr, "Making table!\n");
		/* get your own value and fill the table with it */
		char *your_key = "20 bytes 0123456789!";
		char *your_ip = inet_ntoa(serveraddr.sin_addr);
		for (int i = 0; i < NUM_KEYS; i++) {
			memcpy(lookup_table[i], your_key, KEY_SIZE);
			memcpy(lookup_table[i] + KEY_SIZE, your_ip, IP_ADDR_SIZE);
                } 
/*		for (int i = 0; i < NUM_KEYS; i++) {
			printf("Key %d: %s\n", i, lookup_table[i]);
                        }*/
		print_lookup();
        	memcpy(predecessor, your_key, KEY_SIZE);
		memcpy(predecessor + KEY_SIZE, your_ip, IP_ADDR_SIZE);
		//memcpy(self, your_key, KEY_SIZE);
		//memcpy(self, your_ip, IP_ADDR_SIZE);
                // ITEM 1: this doesn't quite work and I'm not sure why

//	}	
	
	// main loop
                while(1){
			int num_sent;
			char user_op;
		
			/*
                         *  recvfrom: receive a UDP datagram from a client    
                         */
			clientlen = sizeof(clientaddr);
			bzero(buf, BUFSIZE);	
			num_sent = recvfrom(sockfd, buf, BUFSIZE, 0,
                                                (struct sockaddr *) &clientaddr, &clientlen);
                        if(num_sent > 0) {
				printf("received message: %s\nnum bytes received: %d\n", buf, num_sent);
				user_op = buf[0];
				switch(user_op) {
	        	        case 1:
					join(buf, clientaddr, sockfd);
                                        break;
                        	case 2:
					update_table(buf, clientaddr, sockfd);
					break;
                                case 4:
					down(buf, clientaddr, sockfd);
					break;
				case 3:
					update_pred(clientaddr);
                    		default:
					break;
                                }
                        }


			if (n < 0)
				error("ERROR in recvfrom");
			
		      	/* echo message */
			/*n = sendto(sockfd, buf, sizeof(buf), 0, 
                                   (struct sockaddr *) &clientaddr, clientlen);
			}
			*/

			/* check for user input */		
			if (inputAvailable()) {
				scanf("%c %s\n", &user_op, &fname);
				if (user_op == 'u')
					fprintf(stderr, "User has chosen to upload file %s\n", fname);
				if (user_op == 'd')
					fprintf(stderr, "User has chosen to download file %s\n", fname);
				if (user_op == 'r')
					fprintf(stderr, "User has chosen to remove file %s\n", fname);
			}
               }
}


int inputAvailable()
{
	struct timeval tv;
	fd_set fds;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);
	select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
	return (FD_ISSET(0, &fds));
}

void print_lookup() {
	//fprintf(stderr, "Self IP: %s\n", KEY_SIZE);
	fprintf(stderr, "Predecessor Key:%s\n", predecessor + KEY_SIZE);
	for(int i = 0; i < NUM_KEYS; i++) {
		fprintf(stderr, "Key %d: %s\n", i, lookup_table[i]);		
        }
}

/*
 * join()
 * Sends the existing table to the new node, then updates table so that successor is new node
 */
int join(char buf[], struct sockaddr_in clientaddr, int sockfd) {
	fprintf(stderr, "Received Join Request\n");
	join_resp new_reply;
	join_req orig_req;

	new_reply.type = 2; /* update table */
	memcpy(new_reply.key, self, KEY_SIZE);
	memcpy(new_reply.tk_in.key, lookup_table[0], KEY_SIZE);
	memcpy(new_reply.tk_in.ip, lookup_table[0]+KEY_SIZE, IP_ADDR_SIZE);
	int num_sent = sendto(sockfd, &new_reply, sizeof(join_resp), 0, (struct sockaddr *)&clientaddr, sizeof(clientaddr));
	if(num_sent < 0)
		return 0;
	fprintf(stderr, "Sending reply - num bytes sent: %d\n", num_sent);

	memcpy(&orig_req, buf, sizeof(join_req));
	/* ITEM 2: why doesn't this work */
//	memcpy(lookup_table[0], &orig_req.key, KEY_SIZE);
	char *new_ip = inet_ntoa(clientaddr.sin_addr);
	if(strcmp(predecessor, self))  {
		fprintf(stderr, "Updating pred because First Node detected\n");
		memcpy(predecessor, &orig_req.key, KEY_SIZE);
		memcpy(predecessor, new_ip, IP_ADDR_SIZE);
        } 	
	memcpy(lookup_table[0] + KEY_SIZE, new_ip, IP_ADDR_SIZE);
	print_lookup();
	return 1;
}

int down(char buf[], struct sockaddr_in client_addr, int sockfd) {
	(void) buf;
	(void) client_addr;
}

/*
 * update_table()
 * Node accepts new key & IP as predecessor as well as table_key for successor
 */
int update_table(char buf[], struct sockaddr_in clientaddr, int sockfd) {
	join_resp jreply;
	fprintf(stderr, "Received Join Response - Updating Table and Pred\n");
	memcpy(&jreply, buf, sizeof(join_resp));
	char *ip = inet_ntoa(clientaddr.sin_addr);
//	for (int i = 0; i < NUM_KEYS; i++) {
                memcpy(lookup_table[0], jreply.tk_in.key, KEY_SIZE);
		memcpy(lookup_table[0] + KEY_SIZE, jreply.tk_in.ip, IP_ADDR_SIZE);
//        } 
	memcpy(predecessor, jreply.key, KEY_SIZE);
	memcpy(predecessor + KEY_SIZE, ip, IP_ADDR_SIZE);
	print_lookup();
	join_pred(sockfd);
}

int join_pred(int sockfd) {
	fprintf(stderr, "Sending Pred Response\n");
	struct sockaddr_in clientaddr;
	pred_resp preply;
	preply.type = 3;
	clientaddr.sin_family = AF_INET;
	clientaddr.sin_port = htons(9060); // change this to be the actual port
	inet_aton(self, &clientaddr.sin_addr);
	int num_sent = sendto(sockfd, &preply, sizeof(pred_resp), 0, (struct sockaddr *)&clientaddr,sizeof(clientaddr));
        return 1;
}

int update_pred(struct sockaddr_in clientaddr) {
	fprintf(stderr, "Received Pred Response\n");
	char *ip = inet_ntoa(clientaddr.sin_addr);
	memcpy(predecessor + KEY_SIZE, ip, IP_ADDR_SIZE);
        return 1;
}
