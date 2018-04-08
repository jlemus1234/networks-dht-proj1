/*
 * a4.c - A simple UDP echo server
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

int inputAvailable();


/*
 * error - wrapper for perror
 */
void error(char *msg) {
	perror(msg);
	exit(1);
}

typedef struct __attribute__((__packed__)) read_req{
	char type;
	char window_size;
	char fname[20];
}read_req; 

typedef struct __attribute__((__packed__)) data_mess{
	char type;
	char seq_num;
	char data[512];
}data_mess; 

typedef struct __attribute__((__packed__)) ack_mess{
	char type;
	char seq_num; 
}ack_mess; 

typedef struct __attribute__((__packed__)) err_mess{
	char type;
}err_mess; 

typedef struct __attribute__((__packed__)) table_req{
	char data[512];
}table_req;

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
	read_req req;
	data_mess data;
	ack_mess ack;
	err_mess err; 
	int window_size, seq_num;
	char fname[20];

	err.type = 4;
	data.type = 2;

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
         * build the server's Internet address  --- this might be wrong for the 3 case - infact i think its wrong
         */
	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	//if (argc == 2) {
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
        /*} else {
		/* gethostbyname: get the server's DNS entry */
	/*	hostname = argv[1];
		server = gethostbyname(hostname);
		if (server == NULL) {
			fprintf(stderr, "ERROR, no such host as %s\n", 
			hostname);
			exit(0);
        	}
		bcopy((char *) server->h_addr,
              		(char *)&serveraddr.sin_addr.s_addr, 
			server->h_length);	
        }
	*/	 
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
	
	
	
	if (argc == 4) {
		// Get table from peer
		char buf[1024];
		table_req newreq;
		int portno = atoi(argv[3]);
		memcpy(newreq.data, "Hello. This is a table request.", 512);
		destaddr.sin_family = AF_INET;
		network = gethostbyname(argv[2]);
		if (network == NULL) {
			fprintf(stderr, "ERROR, no such host as %s\n", network);
			exit(0);
                }
		bcopy((char *) network->h_addr,
                      (char *)&destaddr.sin_addr.s_addr, network->h_length);
		destaddr.sin_port = htons(portno);
		sendto(sockfd, &newreq, sizeof(table_req), 0, (struct sockaddr *)&destaddr, sizeof(destaddr));
		recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL);
		printf("Reply from peer: %s of size %d\n", buf, n);
	}


      	//else /*set up table by yourself*/
		
	
	
                while(1){
			int num_sent;
			char user_op;
			//fprintf(stderr, "*Checking for message from client*\n");
			/*
                         *  recvfrom: receive a UDP datagram from a client    
                         */
			clientlen = sizeof(clientaddr);
			bzero(buf, BUFSIZE);	
			num_sent = recvfrom(sockfd, buf, BUFSIZE, 0,
                                                (struct sockaddr *) &clientaddr, &clientlen);
                        if(num_sent > 0) {
				printf("received message: %s\nnum bytes received: %d\n", buf, num_sent);
			if (n < 0)
				error("ERROR in recvfrom");
			fprintf(stderr, "received read req\n");

		      	/* echo message */
			n = sendto(sockfd, buf, sizeof(buf), 0, 
                                   (struct sockaddr *) &clientaddr, clientlen);
			}


			//fprintf(stderr, "*Checking for user input*\n");
			/* check for user input */		
			if (inputAvailable()) {
				scanf(" %c %s\n", &user_op, &fname);
				//fflush(stdin);
				if (user_op == 'u')
					fprintf(stderr, "User has chosen to upload file %s\n", fname);
				if (user_op == 'd')
					fprintf(stderr, "User has chosen to download file %s\n", fname);
				if (user_op == 'r'){
                                        fflush(stdin);
					fprintf(stderr, "User has chosen to remove file %s\n", fname);
                                }
			}
               }


















	/*
         * main loop: wait for a datagram, then echo it
         */
/*	clientlen = sizeof(clientaddr);
	while (1) {
		/*
                 * recvfrom: recieve a UDP datagram from a client
                 */
/*		bzero(buf, BUFSIZE);
		int num_sent = recvfrom(sockfd, buf, BUFSIZE, 0,
                             (struct sockaddr *) &clientaddr, &clientlen);
		printf("num bytes received: %d\n",num_sent);
		if (n < 0)
			error("ERROR in recvfrom");
		fprintf(stderr, "received read req\n");		
//		if (n != 22)
//			close(&clientaddr);

		if (num_sent == 22) {
		
/* HANDLE BULLSHIT */



		/*
                 * gethostbyaddr: determine who sent the datagram
                 */
/*		hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
                	sizeof(clientaddr.sin_addr.s_addr), AF_INET);

		if (hostp == NULL) 
			error("ERROR on gethostbyaddr");
		hostaddrp = inet_ntoa(clientaddr.sin_addr);
		if (hostaddrp == NULL)
			error("ERROR on inet_ntoa\n");
		printf("server received read req from %s (%s)\n",
                       hostp->h_name, hostaddrp);
		printf("server received %d/%d bytes: %s\n", strlen(buf), n, 
			buf);

		/* The first message must be a read request */
/*		memcpy(&req, buf, sizeof(read_req));
		if(req.type != 1)
			break;
		window_size = req.window_size;
		memcpy(fname, req.fname, strlen(req.fname));
		
		/* handle file io */
/*		if(access(fname, F_OK) != -1) {
			fprintf(stderr, "Sending file:%s\n", fname);
			FILE *fp;
			char buf[512];
			fp = fopen(fname, "r");
			struct stat fileStat;
			stat(fname, &fileStat);
			int fsize = fileStat.st_size;
			data.type = 2;
			data.seq_num = seq_num;
			while (fsize >= 512){
				fread(buf, sizeof(buf), 1, fp);
				memcpy(data.data, buf, sizeof(buf));
				n = sendto(sockfd, data, sizeof(data), 0,
                                    (struct sockaddr *) &clientaddr, clientlen);
				//n = sendto(sockfd, &data, sizeof(data), 0,
                                //    (struct sockaddr *) &clientaddr, clientlen);
				if (n < 0)
					error("ERROR in sendto");	
				fsize = fsize - 512; 
				fprintf(stderr, "%d", fsize);
                        }
			if (fsize > 0) {
			       	bzero(buf, sizeof(buf));
				fprintf(stderr, "fsize: %d\n", fsize);
				fread(buf, fsize, 1, fp);
				memcpy(data.data, data, fsize);
				n = sendto(sockfd, data, fsize+2, 0,
                                    (struct sockaddr *) &clientaddr, clientlen);
                        }
			fclose(fp);
                } else {
			n = sendto(sockfd, &err, sizeof(err_mess), 0,
                                   (struct sockaddr *) &clientaddr, clientlen);
			if (n < 0)
				error("ERROR in sendto");
		}	
		
		fprintf(stderr, "Done sending file\n");
		/*
                 * sendto: echo the input back to the client
                 */
/*		n = sendto(sockfd, buf, strlen(buf), 0, 
                           (struct sockaddr *) &clientaddr, clientlen);
		if (n < 0)
			error("ERROR in sendto");
		}
*/		/* what happens if the first read is not a read request? */
/*		}
                }*/
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
