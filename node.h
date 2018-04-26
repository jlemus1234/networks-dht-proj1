
#ifndef NODE_INCLUDED
#define NODE_INCLUDED

#define TRUE 1
#define FALSE 0

#pragma pack(1)
typedef struct com
{
	int type;
	int stat;
        int sourcePort;
	int port2;
	int length;
	char sourceIP[16];
	char IP2[16];
	
        char reqHash[41];
	char data[512];
} com;

typedef struct node 
{
	char ipAdd[16];
	int port;
	char hash[41];

        char ipSucc[16];
	int portSucc;
	char hashSucc[41];

	char ipPred[16];
	int portPred;
	char hashPred[41];
} node;

//pthread_mutex_t modTableState;
//pthread_mutex_t checkDL;


// Handles recieving packets 
// Bootstraps the node using a given hostname or hostport or 
// becomes the first node in a new network
void network();

// Processes user commands for a specific node
// u -- Upload a file 
// d -- Download a file
// s -- 
// l -- Leave the network, transferring all data and maintaining structure of DHT
// t -- 
// p -- Print the contents of a node's data array
// c -- Get a specific element from a node's data table using a hexadecimal hash
void* getInput();

// Transfer data contained in a char array of size length to a 
// specified hostname and port
int pass(int length, char* data, char* hostname, int hostport);

// Print the contents of a node structure to stderr. 
void printNode (node* self);
#endif
