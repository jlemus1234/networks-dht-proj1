#ifndef FILEGEN_INCLUDED
#define FILEGEN_INCLUDED

#include "node.h"

static const int HEXHASHLEN = 40;

typedef struct dataPair {
        char* key; 
	char* data; 
	int len;
} dataPair;

typedef struct dataArr {
	size_t max;
	size_t used;
	dataPair** pairs;
} dataArr;


// Initialize a strucure that will contain a hashed key and data pair
dataPair* initdataPair();

// Initialize a structure that will contain all dataPairs a node has recieved
dataArr* initdataArr();

// Expand a data array 
void growDataArr(dataArr *arr);

// prints the keys and corresponding data for every pair in a data array
void printDataArr(dataArr* arr);

// Insert a new dataPair into a data array
// Returns 0 if an entry was successfully inserted
// 	   1 if an entry was already present
int insertPair(dataArr* arr, dataPair* pair);

// Retrieves a data pair with a matching key from a data array 
dataPair* getData(dataArr *arr, char* key); 

// Splits an input file into 512 byte chunks and generates a data pair
// for each, inserting each data pair into arr.
void inputFile(dataArr *arr, char* filename, node* self);

// Frees a data array and all of its contained data pairs
void freedataArr(dataArr *arr);

// Sends all data pairs with a key that is greater than the node
// specified in self to the node's successor
void joinDataSplit(dataArr *arr, node *self);

// Sends all data pairs to the node's successor
void leaveDataTransfer(dataArr *arr, node *self);



#endif
