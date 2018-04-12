
#ifndef NODE_INCLUDED
#define NODE_INCLUDED

#define TRUE 1
#define FALSE 0

#pragma pack(1)
typedef struct com
{
	char type;
	char stat;
	char sourceIP[15];
	int sourcePort;
	int length;
        char reqHash[40];
	char data[512];
} com;

typedef struct node {
	//unsigned long ipAdd;
	//char ipAdd[15];
	char ipAdd[15];
	int port;
	char hash[40];

        char ipSucc[15];
	int portSucc;
	char hashSucc[40];

	char ipPred[15];
	int portPred;
	char hashPred[40];
} node;

void error(char *msg);
void network();
void* getInput();
int hex2int(char ch);
int greaterThanHash(char *hash1, char *hash2);
pthread_mutex_t modTableState;

#endif
