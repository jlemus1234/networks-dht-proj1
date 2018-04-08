#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

static const int CHUNKSIZE = 512; // bytes
char* hashData(char* data);
char* hashNode (char* ip, int port, int virtual);
char* hash2Hex (unsigned char* hash);
int hex2int(char ch);
int greaterThanHash(char *hash1, char *hash2);

#define TRUE 1
#define FALSE 0

/*
int main (int argc, char *argv[]){
	// Printing out hashes
	// Testing node, port, num hashing
	fprintf(stderr, "Hashing node info\n");
        char* test = hashNode("54.208.202.125", 1, 4);
	unsigned char* test2 = hashNode("54.208.202.125", 1 , 1);
	fprintf(stderr, "test in main:\n%s\n", test);

	// Testing 512 byte data hashing
	fprintf(stderr, "Hashing data chunk\n");
	char* data = malloc(CHUNKSIZE * sizeof(char));
        memset(data, 0, CHUNKSIZE);
	data = "ycNZMUqDj88tY5E1c1z5CKbtpiyNa4PENlikkTVolBzyzgXa3ancPhptTBV5fHNeQ248litoiWPoBCwL5CjBsbXub8lbrroB2kZjYMPwhayEvJuYOhgQQbHMOsugoRGT0HzbNzkSyzTbnaT37hUhwuKFyJ6uy7PZzKV2c4QJwcGHn61U4Hjfn5obyowu4QYfQkGR9Y53tyzqxaBSMs4fRJPjpEsCuEVN6Ul9CYLKefXhJQZtHvLyEhgUOC24VLDkKW9AvOzZIKESSmTz3A2ajtTmem8OG6YPrih79fasO4Jxgolnsf6Q3aUI4XMAuDkL9yYj46GIFBWiiaBPUJoBBzaq8asnnqEM0YgzPPNO2lNRe0lJRSFx0MpweRDDRgA6DMLZM392qxPXRmr4PCjZZsUxFvpPEE6rRjDXvkEN4Y7N0K6VtMuopuS3fhf7sJY8LSf4SUF0nyxsU5f6eOaakctlmY9RYbT9NemXtznuG3bzIPeXetUGfTq2JtcTy0GB";
	fprintf(stderr, "data: %s\n", data);
	char* test3 = hashData(data);
  	fprintf(stderr,"test3:\n%s\n", test3);

	// how to compare hashes
	char hash1 [40] = "748EE2186D7DEA549BC0A40A984875007C7EABF0"; //k
	char hash2 [40] = "F2859773DAFDD550B1089C9416AA9CE4205462CA"; //without
	if(greaterThanHash(hash1, hash2)){
                fprintf(stderr, "hash1 is greater\n");
        }else {
                fprintf(stderr, "hash2 is greater\n");
        }
        return 0;
}
*/
char* hashData (char* data){
	int length = 512;
	unsigned char* hash = malloc(SHA_DIGEST_LENGTH);
	SHA1(data, length, hash);
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

	char data[length];
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
	// convert to 40 byte hex and print
	char *out = (char *) malloc( sizeof(char) * ((SHA_DIGEST_LENGTH  *2)+1) );
  	char *p = out;
  	int i;
  	for ( i = 0; i < SHA_DIGEST_LENGTH; i++, p += 2 ) {
    		snprintf ( p, 3, "%02x", hash[i] );
  	}
	free(hash);
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

int greaterThanHash(char *hash1, char *hash2)
{
	int length = 40;
	int greater = FALSE;
	for(int i = 0; i < length; i++){
                char char1 = hash1[i];
		char char2 = hash2[i];
		int num1 = hex2int(char1); 
                int num2 = hex2int(char2);
		fprintf(stderr, "(%c %i),(%c %i)\n", char1, num1, char2, num2);
		if (num1 > num2){
                        return TRUE;
                }else if(num1 == num2){
                        continue;
                }else{
                        return FALSE;
                }
        }
	return FALSE;
}
