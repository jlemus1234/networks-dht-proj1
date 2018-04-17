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



QE* initQE(char* chunkhash);
DLQ* initDLQ();
void growDLQ(DLQ *q);
int checkDLQ(dataArr *arr, DLQ* q);
void printDLQueue(DLQ* q);
void beginDL(dataArr *arr, DLQ *q, char* filename, node *self);
void writeDL(dataArr *arr, DLQ *q);
void insertDLQ(DLQ *q, char *hash, int status);
void freeDLQ(DLQ *arr);



#endif
