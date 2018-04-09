#ifndef HASHING_INCLUDED
#define HASHING_INCLUDED

#define TRUE 1
#define FALSE 0

static const int CHUNKSIZE = 512; // bytes
char* hashData(char* data);
char* hashNode (char* ip, int port, int virtual);
char* hash2Hex (unsigned char* hash);
int hex2int(char ch);
int greaterThanHash(char *hash1, char *hash2);

#endif
