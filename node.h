
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

typedef struct node {
	//unsigned long ipAdd;
	//char ipAdd[15];
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

//void error(char *msg);
void network();
void* getInput();
int hex2int(char ch);
int greaterThanHash(char *hash1, char *hash2);
pthread_mutex_t modTableState;

#endif
