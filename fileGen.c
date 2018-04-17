#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include "fileGen.h"
#include "hashing.h"


void downloadFile(dataArr *arr, char* filename);

char *newFname(char *fname, char *fname2);


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
//        fprintf(stderr,"Not yet implemented\n");
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
//	fprintf(stderr, "grow called\n");
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
	//fprintf(stderr, "%zu, %zu\n", arr->used, arr->max);
	// scan array for matching entry
	dataPair *getDataResult = getData(arr, pair->key);
        if(getDataResult != NULL){
		fprintf(stderr, "MATCH FOUND\n COLLISION\n");
                return 1;
        }

       	for(size_t i = 0; i < arr->max; i++){
                if(arr->pairs[i]->key == NULL){
//			fprintf(stderr, "Index value: %d", i);
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
//	printDataArr(arr);
        return 0;
}


dataPair* findData(dataArr *arr, char* key) {
	dataPair *pair = getData(arr, key);
	return pair;
}

// returns a c
dataPair* getData(dataArr *arr, char* key){ // change equals to strcmp in body
//	fprintf(stderr, "in getDATA\n");
	if (arr == NULL || key == NULL){
		fprintf(stderr, "\nNULL pointer passed to insertPair\n");
                exit(1);
        }
	dataPair* target = NULL;
      	for(size_t i = 0; i < arr->max; i++){
                if(arr->pairs[i] != NULL && arr->pairs[i]->key != NULL && arr->pairs[i]->data != NULL){
//                        fprintf(stderr, "%d, %zu\n", i, arr->max);
                        if(memcmp(arr->pairs[i]->key, key, HEXHASHLEN) == 0){// is equal? memcomp instead?
				
                                target = arr->pairs[i];
                                return target;
                        }
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
	//fprintf(stderr, "Not yet implemented: Missing meaningful hashes for chunks\n");
	/* Open the file and begin reading it, at 512 byte intervals */
        FILE *reqFile;
	//FILE *hashes; 
        reqFile = fopen(filename, "r");
	FILE *hashes = fopen ("hashes", "w+");
        if(reqFile == NULL){
                fprintf(stderr, "File not found\n");
        }


	char buff [CHUNKSIZE + 1];
	memset(buff, '\0', CHUNKSIZE + 1);
	int readLen;
	while((readLen = fread(buff, 1, CHUNKSIZE, reqFile)) > 0){
//                fprintf(stderr, "%s", buff);
                dataPair pair;
                //pair.key = "356A192B7913B04C54574D18C28D46E6395428AB"; // Needs to be an actual hash
		pair.key = hashData(&buff[0]);
                pair.data = buff;
		pair.len = readLen;
                insertPair(arr, &pair);
//		dataPair *p2;
//		p2 = getData(arr, pair.key);
//		fprintf(stderr, "Inserted data: %s", p2->data);
//		fprintf(stderr, "We made it\n");
		fwrite(pair.key, sizeof(char), HEXHASHLEN, hashes);
		fwrite("\n", sizeof(char), 1, hashes);
		
                memset(buff, '\0', CHUNKSIZE);
//		fprintf(stderr, "\nFinished inserting\n");
        }
        fclose(reqFile);
	fclose(hashes);
}


void downloadFile(dataArr *arr, char* filename){
	//fprintf(stderr, "Not yet implemented: Missing meaningful hashes for chunks\n");
	/* Open the file and begin reading it, at 512 byte intervals */
        FILE *outFile;
	char *key;
//	char *newOutput = newFname(filename, "output.txt\n");
        outFile = fopen("output.txt", "w+");
	FILE *hashes = fopen ("hashes", "r");
        if(outFile == NULL){
                fprintf(stderr, "File not found\n");
        }


	char buff [HEXHASHLEN];
//	memset(buff, '\0', HEXHASHLEN);
	int readLen;
//	while((readLen = fread(buff, 1, HEXHASHLEN+1, hashes)) > 0){
 	while((readLen = fread(buff, 1, HEXHASHLEN, hashes)) > 0) {
                //fprintf(stderr, "key: %s\n", buff);
                dataPair *pair;
                //pair.key = "356A192B7913B04C54574D18C28D46E6395428AB"; // Needs to be an actual hash
		key = buff;
                pair = findData(arr, key);
		
	       	fprintf(stderr, "data: %s\n", pair->data);
		fwrite(pair->data, sizeof(char), readLen, outFile);
                fread(buff, 1, 1, hashes);
		fprintf(stderr, "\nFinished outputting\n");
        }
        
        fclose(outFile);
	fclose(hashes);
}

/*
void downloadFile(dataArr *arr, char* filename){
	//fprintf(stderr, "Not yet implemented: Missing meaningful hashes for chunks\n");
	/* Open the file and begin reading it, at 512 byte intervals */
/*        FILE *outFile;
	char *key;
//	char *newOutput = newFname(filename, "output.txt\n");
	fprintf(stderr, "input filename: %s\n", filename);
	fprintf(stderr, "output filename: %s\n", newOutput);
        outFile = fopen(newOutput, "w+");
	FILE *hashes = fopen (filename, "r");
        if(outFile == NULL){
                fprintf(stderr, "File not found\n");
        }


	char buff [HEXHASHLEN];
//	memset(buff, '\0', HEXHASHLEN);
	int readLen;
//	while((readLen = fread(buff, 1, HEXHASHLEN+1, hashes)) > 0){
 	while((readLen = fread(buff, 1, HEXHASHLEN, hashes)) > 0) {
                //fprintf(stderr, "key: %s\n", buff);
                dataPair *pair;
                //pair.key = "356A192B7913B04C54574D18C28D46E6395428AB"; // Needs to be an actual hash
		key = buff;
                pair = findData(arr, key);
		
	       	fprintf(stderr, "data: %s\n", pair->data);
		fwrite(pair->data, sizeof(char), readLen, outFile);
                fread(buff, 1, 1, hashes);
		fprintf(stderr, "\nFinished outputting\n");
        }
        
        fclose(outFile);
	fclose(hashes);
        }*/

char *newFname(char *fname, char *fname2){
	char *buf = malloc(sizeof(char)*30);
	int i = 0;
	int j = 0;
	while(i < 5) {
		buf[i] = fname[i];
		i++;
        }
	while(fname2[j] != '\n') {
		buf[i] = fname2[j];
		i++;
		j++;
        }
	char buff[i];
	memcpy(buff, buf, i - 1);
	memset(buff, '\0', i); 
	fprintf(stderr, "Fname: %s", buf);
	return buf;
}
