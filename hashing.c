#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include "hashing.h"

char* hashData (char* data){
	int length = 512;
	unsigned char* hash = malloc(SHA_DIGEST_LENGTH);
	SHA1((unsigned char *)data, length, hash);
	//return hash;
	return hash2Hex(hash);
}

char* hashNode (char* ip, int port, int virtual){
        int sizep = snprintf(NULL, 0, "%d", port);
	int sizev = snprintf(NULL, 0, "%d", virtual);
	int sizeip = strlen(ip);
	int length = sizep + sizev + sizeip;
	char pstring[sizep]; 
	char vstring[sizev];
	sprintf(pstring, "%d", port);
	sprintf(vstring, "%d", virtual);

	unsigned char data[length]; // convert to unsigned char and retest later
        memset(data, 0, length);
	memcpy(data, ip, sizeip);
	memcpy(data + sizeip, pstring, sizep);
        memcpy(data + sizeip + sizep, vstring, sizev);
	data[length] = 0;
	fprintf(stderr, "hash target: %s\n", data);


	unsigned char* hash = malloc(SHA_DIGEST_LENGTH);
	SHA1(data, length, hash);
	return hash2Hex(hash);
}


// Accepts a SHA1 hash of length 20
// Returns a string of length 40 containing a hexadecimal number
char* hash2Hex (unsigned char* hash){
	fprintf(stderr, "converting to hex\n");
	// convert to 40 byte hex and print
	char *out = (char *) malloc( sizeof(char) * ((SHA_DIGEST_LENGTH  *2)+1) );
  	char *p = out;
  	int i;
  	for ( i = 0; i < SHA_DIGEST_LENGTH; i++, p += 2 ) {
    		snprintf ( p, 3, "%02x", hash[i] );
  	}
	free(hash);
	fprintf(stderr, "finished conversion\n");
	return out;
}


int hex2int(char ch)
{
        if (ch >= '0' && ch <= '9')
                return ch - '0';
        if (ch >= 'A' && ch <= 'F')
                return ch - 'A' + 10;
        if (ch >= 'a' && ch <= 'f')
                return ch - 'a' + 10;
        return -1;
}

// returns 1 if the hash1 is greater
// returns 0 if hash2 is greater
// returns 2 if the hashes are equal
int greaterThanHash(char *hash1, char *hash2)
{
	int length = 40;
	//int greater = FALSE;
	for(int i = 0; i < length; i++){
                char char1 = hash1[i];
		char char2 = hash2[i];
		int num1 = hex2int(char1); 
                int num2 = hex2int(char2);
		//fprintf(stderr, "(%c %i),(%c %i)\n", char1, num1, char2, num2);
		if (num1 > num2){
                        return TRUE;
                }else if(num1 == num2){
                        continue;
                }else{
                        return FALSE;
                }
        }
	return 2; // looped through entire hash and was equal. 
}
