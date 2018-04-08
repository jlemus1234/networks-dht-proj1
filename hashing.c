#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

static const int CHUNKSIZE = 512; // bytes
char* hash(char* data);
char* nodeHash (char* ip, int port, int virtual);
int main (int argc, char *argv[]){

	// Testing node, port, num hashing
	fprintf(stderr, "Hashing node info\n");
        unsigned char* test = nodeHash("54.208.202.125", 1, 4);
	unsigned char* test2 = nodeHash("54.208.202.125", 1 , 1);

	// convert to 40 byte hex and print
	char *out = (char *) malloc( sizeof(char) * ((SHA_DIGEST_LENGTH  *2)+1) );
  	char *p = out;
  	int i;
  	for ( i = 0; i < SHA_DIGEST_LENGTH; i++, p += 2 ) {
    		snprintf ( p, 3, "%02x", test[i] );
  	}
  	fprintf(stderr,"first hash:\n%s\n", out);

  	char *out2 = (char *) malloc( sizeof(char) * ((SHA_DIGEST_LENGTH  *2)+1) );
  	char *p2 = out2;
  	int q;
  	for ( q = 0; q < SHA_DIGEST_LENGTH; q++, p2 += 2 ) {
    		snprintf ( p2, 3, "%02x", test2[q] );
  	}
  	fprintf(stderr,"second hash:\n%s\n", out2);

	free(test);
	free(out);
	free(test2);
	free(out2);


	// Testing 512 byte data hashing
	fprintf(stderr, "Hashing data chunk\n");
	char* data = malloc(CHUNKSIZE * sizeof(char));
        memset(data, 0, CHUNKSIZE);
	data = "ycNZMUqDj88tY5E1c1z5CKbtpiyNa4PENlikkTVolBzyzgXa3ancPhptTBV5fHNeQ248litoiWPoBCwL5CjBsbXub8lbrroB2kZjYMPwhayEvJuYOhgQQbHMOsugoRGT0HzbNzkSyzTbnaT37hUhwuKFyJ6uy7PZzKV2c4QJwcGHn61U4Hjfn5obyowu4QYfQkGR9Y53tyzqxaBSMs4fRJPjpEsCuEVN6Ul9CYLKefXhJQZtHvLyEhgUOC24VLDkKW9AvOzZIKESSmTz3A2ajtTmem8OG6YPrih79fasO4Jxgolnsf6Q3aUI4XMAuDkL9yYj46GIFBWiiaBPUJoBBzaq8asnnqEM0YgzPPNO2lNRe0lJRSFx0MpweRDDRgA6DMLZM392qxPXRmr4PCjZZsUxFvpPEE6rRjDXvkEN4Y7N0K6VtMuopuS3fhf7sJY8LSf4SUF0nyxsU5f6eOaakctlmY9RYbT9NemXtznuG3bzIPeXetUGfTq2JtcTy0GB";
	fprintf(stderr, "data: %s\n", data);

	unsigned char* test3 = hash(data);
  	char *out3 = (char *) malloc( sizeof(char) * ((SHA_DIGEST_LENGTH  *2)+1) );
  	char *p3 = out3;
  	int q3;
  	for ( q3 = 0; q3 < SHA_DIGEST_LENGTH; q3++, p3 += 2 ) {
    		snprintf ( p3, 3, "%02x", test3[q3] );
  	}
  	fprintf(stderr,"data hash:\n%s\n", out3);


        return 0;
}

char* hash (char* data){
	int length = 512;
	unsigned char* hash = malloc(SHA_DIGEST_LENGTH);
	SHA1(data, length, hash);
	return hash;
}

char* nodeHash (char* ip, int port, int virtual){
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
        return hash;
}

