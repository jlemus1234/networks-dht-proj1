
#ifndef NODE_INCLUDED
#define NODE_INCLUDED

#define TRUE 1
#define FALSE 0

#pragma pack(1)
typedef struct com
{
	char type;
	int sourceIP;
	int sourcePort;
	int length;
        char reqHash[40];

} com;

typedef struct node {
	int ipAdd;
	int port;

        int ipAddSucc;
	int portSucc;

	int ipAddPred;
	int portPred;
} node;

void error(char *msg);
void network();
void* getInput();
int hex2int(char ch);
int greaterThanHash(char *hash1, char *hash2);
pthread_mutex_t modTableState;

#endif
