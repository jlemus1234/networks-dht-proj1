#include <stdio.h>
#include <string.h>
#include "hashing.h"

#define num_entries 5
#define HASH_LEN 40
int max_index = num_entries - 1;

int largest_node; /* might not need */
node *self;
node finger_table[num_entries];
/*int sockfd;
struct sockaddr_in serveraddr;
/*
void init(int portno, str hostname, int hostport);
void init(int portno);
*/


int next_hop(char key[HASH_LEN + 1]);


int main()
{
	/* Setting up network */
		// Check for arguments
	int port = strtol(argv[1], NULL, 10);
	char *hostname;
	int  hostport;
        if (argc != 2 && argc != 4) {
		fprintf(stderr, "usage: %s <port1> [IP address] [port2] \n", argv[0]); 
		fprintf(stderr, "where port1 is the port to run the program");
		fprintf(stderr, " on and port2 is the port number of a node ");
		fprintf(stderr, "on the network. IP Address is the IP of an ");
		fprintf(stderr, "existing node\n");
		exit(1);
        }
	
        initNode(self, port);
	if (argc == 2) {
		largest_node = 1; /* might not need */ 
		initTable();
	} else if(argc == 4){
                hostname = argv[2];
                hostport = strtol(argv[3], NULL, 10);
		largest_node = 0; /* might not need */
        }

	
	
	
	
	/*int num = 1;

	self = "0000000000000000000000000000000000000000\0";
	largest_node = 1;
	pred = 0;

	int split_val = 6;

	//fprintf(stderr, "Making Table\n");
/*	for(int i = split_val; i < num_entries; i++) {
		finger_table[i] = num;
		num *= 2;
        }

	self = num;

	for(int i = 0; i < split_val; i++) {
		finger_table[i] = num;
		num *= 2;
                }*/
/*
	memcpy( finger_table[0], "1234567890123456789012345678901234567890\0", HASH_LEN + 1);
	memcpy( finger_table[1], "2234567890123456789012345678901234567890\0", HASH_LEN + 1); 
	memcpy( finger_table[2], "3234567890123456789012345678901234567890\0", HASH_LEN + 1); 
	memcpy( finger_table[3], "4234567890123456789012345678901234567890\0", HASH_LEN + 1); 
	memcpy( finger_table[4], "5234567890123456789012345678901234567890\0", HASH_LEN + 1); 


	for(int i = 0; i < num_entries; i++) {
		fprintf(stderr, "entry %d: %s\n", i, finger_table[i]);
        }
	fprintf(stderr, "Next hop for %s is %s\n",  "4000000000000000000000000000000000000000\0", finger_table[next_hop( "4000000000000000000000000000000000000000\0")]);

	
/*	if(argc == 2) {
		init(argv[1]);
        }
	else if(argc == 4) {
		init(strtol(argv[1]), argv[2], strtol(argv[3]));
        }
	else
                fprintf("Usage: "); /************************** add usage message ******************/
	return 0;
}
/*
void init(int portno) {
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sock_fd < 0)
		error("ERROR OPENING SOCKET");
	server 
}

void init(int portno, str hostname, int hostport) {
}
*/

/* lookup logic - takes key and returns the index of the next hop */
int next_hop(char key[40]) {
	int index = 0;

	if (greaterThanHash(key,  self)) {
		while(greaterThanHash(key, finger_table[index + 1]) && index < max_index) {
			index++;
//			fprintf(stderr, "key was larger than %d\n", finger_table[index]);
			if (greaterThanHash(finger_table[index], finger_table[index + 1])) 
				break;
                }
		return index;
	}
	/* key < self */
		index = max_index;
		while(greaterThanHash(finger_table[index], key) && index > 1) {
			index--;
//			fprintf(stderr, "key was less than %d\n", finger_table[index]);
                }
		return index;
}


/* Node setup */
void initNode (node* self, int myPort){
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

void printNode (node* self){
	fprintf(stderr, "\nPrinting node:\n");
        fprintf(stderr, "self: %s, %i\n%s\n", self->ipAdd , self->port,     self->hash);
        fprintf(stderr, "succ: %s, %i\n%s\n", self->ipSucc, self->portSucc, self->hashSucc);
        fprintf(stderr, "pred: %s, %i\n%s\n", self->ipPred, self->portPred, self->hashPred);
	fprintf(stderr, "\n");

}

/* only called if the first node in the network */
void initTable(){
	for(int i = 0; i < max_index; i++) {
		memcpy(finger_table[i], self.hash, HASH_LEN + 1);
	}
	printTable();
}

void printTable() {
	fprintf(stderr, "Printing table:\n");
	for(int i = 0; i < max_index; i++) {
		fprintf(stderr, "entry %d: %s\n", i, finger_table[i]);
	}	
}
