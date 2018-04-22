#ifndef HASHING_INCLUDED
#define HASHING_INCLUDED

#define TRUE 1
#define FALSE 0

static const int CHUNKSIZE = 512; // bytes

// Accepts a 512 byte chunk and returns a 40 char 
// hexadecimal SHA1 hash
char* hashData(char* data);

// Accepts an ip address, port number, and a virtual node number
// Returns a 40 char SHA1 hash in hexadecimal format
char* hashNode (char* ip, int port, int virtual);

// Accepts a SHA1 hash of length 20
// Returns a string of length 40 containing a hexadecimal number
char* hash2Hex (unsigned char* hash);

// Converts a hexadecimal character into its corresponding numerical
// value in a base 10 system
int hex2int(char ch);

// Compares two 40 byte char arrays in hexadecimal format 
// to determine which is greater
// Returns 1 if hash1 is greater
// 	   0 if hash2 is greater
// 	   2 if hash1 and hash2 are equal
int greaterThanHash(char *hash1, char *hash2);

#endif
