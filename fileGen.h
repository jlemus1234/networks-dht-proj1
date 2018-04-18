#ifndef FILEGEN_INCLUDED
#define FILEGEN_INCLUDED

#include "node.h"

//static const int CHUNKSIZE = 512; // bytes
static const int HEXHASHLEN = 40;

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
dataPair* getData(dataArr *arr, char* key); // change equals to strcmp in body
void inputFile(dataArr *arr, char* filename, node* self);
void freedataArr(dataArr *arr);
void joinDataSplit(dataArr *arr, node *self);



#endif
