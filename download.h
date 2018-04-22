#ifndef DOWNLOAD_INCLUDED
#define DOWNLOAD_INCLUDED

#include "node.h"

typedef struct QE {
        char *hash;
	int status;

} QE;

typedef struct DLQ {
        size_t max;
        size_t used;
        QE ** entries;
} DLQ;


// Returns a pointer to a newly initialized queue entry
QE* initQE(char* chunkhash);

// Initialize a download queue, returning a pointer to it
DLQ* initDLQ();

// Expand the download queue 
void growDLQ(DLQ *q);

// Check if a node q has all the chunks required for a download
// Returns 1 if it does, otherwise returns 0
int checkDLQ(dataArr *arr, DLQ* q);

// Prints the contents of a download queue
void printDLQueue(DLQ* q);

// Initializes a download queue and loads it with the hashes contained in a 
// file, filename.
void beginDL(dataArr *arr, DLQ *q, char* filename, node *self);

// Retrieves chunks specified by a DLQ, q, from a dataArr structure and 
// prints a file DLResult. 
// The program exits if this is called without having all the required chunks
void writeDL(dataArr *arr, DLQ *q);

// Insert a queue entry into a DLQ
void insertDLQ(DLQ *q, char *hash, int status);

// Free a DLQ and all of its contained queue entries
void freeDLQ(DLQ *arr);



#endif
