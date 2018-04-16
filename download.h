#ifndef DOWNLOAD_INCLUDED
#define DOWNLOAD_INCLUDED

static const int CHUNKSIZE = 512; // bytes

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
void beginDL(dataArr *arr, DLQ *q, char* filename);
void writeDL(dataArr *arr, DLQ *q);
void insertDLQ(DLQ *q, char *hash, int status);
void freeDLQ(DLQ *arr);



#endif
