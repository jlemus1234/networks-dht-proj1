#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

static const int CHUNKSIZE = 512; // bytes

typedef struct dataPair {
        char* key; // will be a string
	char* data; // the actual torrent file doesn't use this except to create the hash. 
	int len;
} dataPair;

typedef struct dataArr {
	size_t max;
	size_t used;
	dataPair** pairs;
} dataArr;

dataPair* initdataPair();
dataArr* initdataArr();
void growDataArr(dataArr *arr);
dataPair* getPair();
dataArr* removePair();
void printDataArr(dataArr* arr);
int insertPair();
//void hash (char* data);
//void hash ();
char* hash(char* data);
char* nodeHash (char* ip, int port, int virtual);
//void nodeHash (char* ip, int port, int virtual);

int main (int argc, char *argv[]){
//hash();

	// Testing node, port, num hashing
	fprintf(stderr, "Hashing node info\n");
        unsigned char* test = nodeHash("54.208.202.125", 1, 4);
	unsigned char* test2 = nodeHash("54.208.202.125", 1 , 1);

	// convert to 40 byte hex and print
	char *out = (char *) malloc( sizeof(char) * ((20  *2)+1) );
  	char *p = out;
  	int i;
  	for ( i = 0; i < 20; i++, p += 2 ) {
    		snprintf ( p, 3, "%02x", test[i] );
  	}
  	fprintf(stderr,"%s\n", out);

  	char *out2 = (char *) malloc( sizeof(char) * ((20  *2)+1) );
  	char *p2 = out2;
  	int q;
  	for ( q = 0; q < 20; q++, p2 += 2 ) {
    		snprintf ( p2, 3, "%02x", test2[q] );
  	}
  	fprintf(stderr,"%s\n", out2);

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
	//data[CHUNKSIZE] = 0;
	unsigned char* test3 = hash(data);

  	char *out3 = (char *) malloc( sizeof(char) * ((20  *2)+1) );
  	char *p3 = out3;
  	int q3;
  	for ( q3 = 0; q3 < 20; q3++, p3 += 2 ) {
    		snprintf ( p3, 3, "%02x", test3[q3] );
  	}
  	fprintf(stderr,"%s\n", out3);


        return 0;
}



dataPair* initdataPair(){
        dataPair* d = malloc(sizeof(d));
	d->key  = NULL;
	d->len  = 0;
	d->data = NULL;
        return d;
}

dataArr* initdataArr(){
        dataArr* arr = malloc(sizeof(dataArr));
	arr->used = 0;
	arr->max = 10;
	arr->pairs = malloc(sizeof(dataPair*) * arr->max);
	for(int i = 0; i < arr->max; i++){
                arr->pairs[i] = initdataPair();
        }
        return arr;
}


void growDataArr(dataArr *arr){
	fprintf(stderr, "grow called\n");
	int currSize = arr->max;
	int newSize = (arr->max) * 2;
        arr->pairs = realloc(arr->pairs, (sizeof(dataPair*) * newSize));
        arr->max = newSize;

	for(int i = currSize; i < newSize; i++){ //check if currSize or currSize + 1;
		arr->pairs[i] = initdataPair();
	}
}


dataPair* getPair(dataArr* arr){
        return NULL;

}

// returns 0 if an entry was successfully made
// returns 1 if the entry was already present
// returns 2 if the client is found and the specified port doesn't match
/*
int insertPair(dataArr* arr, dataPair* pair){
        if(arr->max == arr->used){
                growDataArr(arr);
        }
	// scan array for matching entry
	int searchResult = searchClientArr(arr, clientID, port);
        if(searchResult != 0){
                return searchResult;
        }

       	for(int i = 0; i < arr->max; i++){
                if(arr->clients[i]->active == 0){
			arr->clients[i]->clientID = malloc((sizeof(char) * (strlen(clientID) + 1)));
			strcpy(arr->clients[i]->clientID, clientID); // copy the thing in, have to malloc space first
			arr->clients[i]->expecting = 0;
                        arr->clients[i]->port = port;
			arr->clients[i]->active = 1;
                        arr->clients[i]->expMessID = 0;
                        break;
                }
        }
	arr->used++;
        return 0;
        return 0;
}
*/

void printDataArr(dataArr *arr){
        fprintf(stderr,"Not yet implemented\n");
}


void inputFile(char* filename){
	fprintf(stderr, "Not yet implemented\n");
}



//void hash (char* data){
char* hash (char* data){
	fprintf(stderr, "in hash\n");
	int length = 512;
//	char data[] = "Hello, world!";
//	size_t length = sizeof(data);

	//unsigned char hash[SHA_DIGEST_LENGTH];
	unsigned char* hash = malloc(SHA_DIGEST_LENGTH);
	SHA1(data, length, hash);
	//fprintf(stderr, hash);
	fprintf(stderr, "finished hash\n");
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


char* nodeHash2 (char* ip, int port, int virtual){
        int sizep = snprintf(NULL, 0, "%d", port);
	int sizev = snprintf(NULL, 0, "%d", virtual);
	int sizeip = strlen(ip);
	char pstring[sizep]; 
	char vstring[sizev];
	sprintf(pstring, "%d", port);
	sprintf(vstring, "%d", virtual);
	
	int length = sizep + sizev + sizeip;
	fprintf(stderr, "lengths: ip %d, p %d, v %d", sizeip, sizep, sizev);
	fprintf(stderr, "length: %d\n", length);
	//char* data = malloc(length);
	char data[length];
        memset(data, 0, length);
	//char data[length];
	memcpy(data, ip, sizeip);
	fprintf(stderr, "hash target: %s\n", data);

	memcpy(data + sizeip, pstring, sizep);
	fprintf(stderr, "hash target: %s\n", data);

        memcpy(data + sizeip + sizep, vstring, sizev);
	data[length] = 0;
	//memcpy(data + sizeip + sizep + sizev, "a", 1);

	fprintf(stderr, "hash target: %s\n", data);


/*	char str[length];
        memset(&str, 0, length);
       	strcat(str, ip);
	strcat(str, pstring);
	strcat(str, vstring);
	fprintf(stderr, "strcat: %s\n", str);
*/

	//unsigned char hash[SHA_DIGEST_LENGTH];
	unsigned char* hash = malloc(SHA_DIGEST_LENGTH);
	SHA1(data, length, hash);
        return hash;

}
