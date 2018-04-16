#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include "fileGen.h"
#include "hashing.h"

/*
int main (int argc, char *argv[]){
	(void) argc;
	(void) argv;
	char* filename = "test4.txt";
	dataArr *arr = initdataArr();
	inputFile(arr, filename);
	printDataArr(arr);
	freedataArr(arr);
        return 0;
}
*/
dataPair* initdataPair(){ 
        dataPair* d = malloc(sizeof(dataPair));
	d->key  = NULL;
	d->len  = 0; // This might need to be malloced; 
	d->data = NULL;
        return d;
}


void freedataArr(dataArr *arr){
        fprintf(stderr,"Not yet implemented\n");
	int numEntries = arr->max;
	for (int i = 0; i < numEntries; i++){
		free(arr->pairs[i]->key);
		free(arr->pairs[i]->data);
		free(arr->pairs[i]);
        }
	free(arr->pairs);
	free(arr);
}

dataArr* initdataArr(){
        dataArr* arr = malloc(sizeof(dataArr));
	arr->used = 0;
	arr->max = 10;
	arr->pairs = malloc(sizeof(dataPair*) * arr->max);
	for(size_t i = 0; i < arr->max; i++){
                arr->pairs[i] = initdataPair();
        }
        return arr;
}


void growDataArr(dataArr *arr){
	fprintf(stderr, "grow called\n");
	size_t currSize = arr->max;
	size_t newSize = (arr->max) * 2;
        arr->pairs = realloc(arr->pairs, (sizeof(dataPair*) * newSize));
        arr->max = newSize;

	for(size_t i = currSize; i < newSize; i++){ //check if currSize or currSize + 1;
		arr->pairs[i] = initdataPair();
	}
}


dataPair* getPair(dataArr* arr){
	(void) arr;
        return NULL;

}

// pass in a length and char pointer for it to copy
// returns 0 if an entry was successfully made
// returns 1 if the entry was already present
// returns 2 if the client is found and the specified port doesn't match

int insertPair(dataArr* arr, dataPair* pair){
	if (arr == NULL || pair == NULL){
		fprintf(stderr, "NULL pointer passed to insertPair\n");
                exit(1);
        }
        if(arr->max == arr->used){
                growDataArr(arr);
        }
	fprintf(stderr, "%zu, %zu\n", arr->used, arr->max);
	// scan array for matching entry
	dataPair *getDataResult = getData(arr, pair->key);
        if(getDataResult != NULL){
		fprintf(stderr, "MATCH FOUND\n COLLISION\n");
                return 1;
        }

       	for(size_t i = 0; i < arr->max; i++){
                if(arr->pairs[i]->key == NULL){
			arr->pairs[i]->key = malloc((sizeof(char) * HEXHASHLEN)); // This parts eh
                        memcpy(arr->pairs[i]->key, pair->key, HEXHASHLEN); // copy the thing in, 
								   // have to malloc space first
			arr->pairs[i]->data = malloc(sizeof(char) * CHUNKSIZE);
		        memcpy(arr->pairs[i]->data, pair->data, CHUNKSIZE);

			arr->pairs[i]->len = pair->len;
                        break;
                }
        }
	arr->used++;
        return 0;
}

// returns a c
dataPair* getData(dataArr *arr, char* key){ // change equals to strcmp in body
	if (arr == NULL || key == NULL){
		fprintf(stderr, "\nNULL pointer passed to insertPair\n");
                exit(1);
        }
	dataPair* target = NULL;
      	for(size_t i = 0; i < arr->max; i++){
                if(arr->pairs[i] != NULL && arr->pairs[i]->key != NULL && arr->pairs[i]->data != NULL){
                        if(memcmp(arr->pairs[i]->key, key, HEXHASHLEN) == 0){// is equal? memcomp instead?
                                target = arr->pairs[i];
                                return target;
                        }else{
                                return target;
                        }
                        break;
                }
        }
        return target;
}

void printDataArr(dataArr *arr){
	size_t numEntries = arr->max;
	for (size_t i = 0; i < numEntries; i++){
                fprintf(stderr, "Entry #%zu:\nkey:%.*s\ndata:%.*s\nlen:%i\n", i, 
                        HEXHASHLEN, arr->pairs[i]->key, CHUNKSIZE ,arr->pairs[i]->data, arr->pairs[i]->len);
        }
}


void inputFile(dataArr *arr, char* filename){
	fprintf(stderr, "Not yet implemented: Missing meaningful hashes for chunks\n");
	/* Open the file and begin reading it, at 512 byte intervals */
        FILE *reqFile;
	//FILE *hashes; 
        reqFile = fopen(filename, "r");

        if(reqFile == NULL){
                fprintf(stderr, "File not found\n");
        }else{
        int nameLen = strlen(filename);
        char hashname [nameLen + 4]; // 1 for null, 3 for .fh
	strcpy(hashname, filename);
	strcat(hashname, ".fh");
	//FILE *hashes = fopen ("hashes", "w+");
        FILE *hashes = fopen(hashname, "w+");


	char buff [CHUNKSIZE + 1];
	memset(buff, '\0', CHUNKSIZE + 1);
	int readLen;
	while((readLen = fread(buff, 1, CHUNKSIZE, reqFile)) > 0){
                fprintf(stderr, "%s", buff);
                dataPair pair;
                //pair.key = "356A192B7913B04C54574D18C28D46E6395428AB"; // Needs to be an actual hash
		pair.key = hashData(&buff[0]);
                pair.data = buff; // is this correct?
		pair.len = readLen;
                insertPair(arr, &pair);
		fwrite(pair.key, sizeof(char), HEXHASHLEN , hashes);
		fwrite("\n", sizeof(char), 1, hashes);
		
                memset(buff, '\0', CHUNKSIZE);
		fprintf(stderr, "\nFinished inserting\n");
        }
        fclose(reqFile);
	fclose(hashes);
        }
}
