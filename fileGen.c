#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <arpa/inet.h>
#include "fileGen.h"
#include "hashing.h"

dataPair* initdataPair(){ 
        dataPair* d = malloc(sizeof(dataPair));
	d->key  = NULL;
	d->len  = 0; 
	d->data = NULL;
        return d;
}


void freedataArr(dataArr *arr){
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
	size_t currSize = arr->max;
	size_t newSize = (arr->max) * 2;
        arr->pairs = realloc(arr->pairs, (sizeof(dataPair*) * newSize));
        arr->max = newSize;

	for(size_t i = currSize; i < newSize; i++){ 
		arr->pairs[i] = initdataPair();
	}
}


// pass in a length and char pointer for it to copy
// returns 0 if an entry was successfully made
// returns 1 if the entry was already present

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

dataPair* getData(dataArr *arr, char* key){ // change equals to strcmp in body
	if (arr == NULL || key == NULL){
		fprintf(stderr, "\nNULL pointer passed to insertPair\n");
                exit(1);
        }
	dataPair* target = NULL;
      	for(size_t i = 0; i < arr->max; i++){
                if(arr->pairs[i] != NULL && arr->pairs[i]->key != NULL && arr->pairs[i]->data != NULL){
                        if(memcmp(arr->pairs[i]->key, key, HEXHASHLEN) == 0){
                                target = arr->pairs[i];
                                return target;
                        }else{
                                //return target;
                        }
                }else{
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


//void inputFile(dataArr *arr, char* filename){
void inputFile(dataArr *arr, char* filename, node* self){
	// Open the file and begin reading it, at 512 byte intervals 
        FILE *reqFile;
        reqFile = fopen(filename, "r");

        if(reqFile == NULL){
                fprintf(stderr, "File not found\n");
        }else{
        int nameLen = strlen(filename);
        char hashname [nameLen + 4]; // 1 for null, 3 for .fh
	strcpy(hashname, filename);
	strcat(hashname, ".fh");
        FILE *hashes = fopen(hashname, "w+");


	char buff [CHUNKSIZE + 1];
	memset(buff, '\0', CHUNKSIZE + 1);
	int readLen;
	while((readLen = fread(buff, 1, CHUNKSIZE, reqFile)) > 0){
                fprintf(stderr, "%s", buff);
                dataPair pair;
		pair.key = hashData(&buff[0]);
                pair.data = buff;
		pair.len = readLen;
                //insertPair(arr, &pair);
		/* UPLOAD TO OTHER NODES START */
		/* determine where to send key */
		com outCom;
                //outCom.type = htonl(2); 
                outCom.stat = htonl(3);
                outCom.type = htonl(3);
                memcpy(outCom.sourceIP, self->ipAdd, 16); 
                outCom.sourcePort = htonl(self->port);
                outCom.length = htonl(pair.len);
		memcpy(outCom.reqHash, pair.key, 41);
                memcpy(outCom.data, pair.data, 512);
                //insertPair(arr, &pair);

		if(greaterThanHash(pair.key, self->hash)) {
			/* key >= selfHash -> Send to Succ */
			fprintf(stderr, "data belongs with successor\n");
			outCom.stat = htonl(1);
			pass(sizeof(outCom), (char*) &outCom, self->ipSucc, self->portSucc);

                }else{ /* key < selfHash -> either held at selfHash or at Pred */
			if(greaterThanHash(pair.key, self->hashPred)){
				fprintf(stderr, "data belongs with me\n");
				insertPair(arr, &pair);
			}else {
				fprintf(stderr, "data belongs with predecessor\n");
                                outCom.stat = htonl(0);
				pass(sizeof(outCom), (char*) &outCom, self->ipPred, self->portPred);
                        }
                }
		/* UPLOAD TO OTHER NODES END */
		fwrite(pair.key, sizeof(char), HEXHASHLEN , hashes);
		fwrite("\n", sizeof(char), 1, hashes);
		
                memset(buff, '\0', CHUNKSIZE);
		fprintf(stderr, "\nFinished inserting\n");
        }
        fclose(reqFile);
	fclose(hashes);
        }
}



void joinDataSplit(dataArr *arr, node* self){
	if (arr == NULL || self == NULL){
		fprintf(stderr, "\nNULL pointer passed to insertPair\n");
                exit(1);
        }

	dataPair* data = NULL;
      	for(size_t i = 0; i < arr->max; i++){
                if(arr->pairs[i] != NULL && arr->pairs[i]->key != NULL && arr->pairs[i]->data != NULL){
		        if(greaterThanHash(self->hash, arr->pairs[i]->key) == 0){
                                data = arr->pairs[i];
				
				com outCom;
                                outCom.type = htonl(2);
                                outCom.stat = htonl(3);
                                memcpy(outCom.sourceIP, self->ipAdd, 16);
                                outCom.sourcePort = htonl(self->port);
                                outCom.length = htonl(data->len);
				memcpy(outCom.reqHash, data->key, 41);
                                memcpy(outCom.data, data->data, 512);
                                pass(sizeof(outCom), (char*) &outCom, self->ipSucc, self->portSucc);

                        }
                }else{
                        break;
                }
        }
}



void leaveDataTransfer(dataArr *arr, node* self){
	if (arr == NULL || self == NULL){
		fprintf(stderr, "\nNULL pointer passed to insertPair\n");
                exit(1);
        }

	dataPair* data = NULL;
      	for(size_t i = 0; i < arr->max; i++){
                if(arr->pairs[i] != NULL && arr->pairs[i]->key != NULL && arr->pairs[i]->data != NULL){
                                data = arr->pairs[i];
				com outCom;
                                outCom.type = htonl(2);
                                outCom.stat = htonl(3);
                                memcpy(outCom.sourceIP, self->ipAdd, 16);
                                outCom.sourcePort = htonl(self->port);
                                outCom.length = htonl(data->len);
				memcpy(outCom.reqHash, data->key, 41);
                                memcpy(outCom.data, data->data, 512);
                                pass(sizeof(outCom), (char*) &outCom, self->ipSucc, self->portSucc);

                }else{
                        break;
                }
        }
}
