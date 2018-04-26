

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <arpa/inet.h>

#include "fileGen.h"
#include "download.h"

QE* initQE(char* chunkhash){
	QE *entry = malloc(sizeof(QE));
        entry-> status = 0;
	entry-> hash = malloc((sizeof(char) * 41));
	memset(entry->hash, '\0', 41);
	memcpy(entry->hash, chunkhash, 41);
        return entry;
}
DLQ* initDLQ(){
        DLQ* q = malloc(sizeof(DLQ));
        q->used = 0;
        q->max = 10;
        q->entries = malloc(sizeof(QE*) * q->max);
	for(size_t i = 0; i < q->max; i++){
                q->entries[i] = NULL;
        }
        return q;
}

void growDLQ(DLQ *q){
        //fprintf(stderr, "grow DLQ called\n");
        size_t currSize = q->max;
        size_t newSize = (q->max) * 2;
        q->entries = realloc(q->entries, (sizeof(QE*) * newSize));
        q->max = newSize;

        for(size_t i = currSize; i < newSize; i++){
                q->entries[i] = NULL;
        }
}

// return 0 if not all files contained
// return 1 if all chunks contained
int checkDLQ(dataArr *arr, DLQ* q){
        //fprintf(stderr, "Checking DLQ\n");
	for(size_t i = 0; i < q->used; i++){
                //if(q->entries[i]->status != 1){
                //        return 0;
                //}
                if(getData(arr, q->entries[i]->hash) == NULL){
                        return 0;
                }
        }
        //writeDL(arr, q);

        return 1;
}



void printDLQueue(DLQ* q){
        (void) q;

}
void beginDL(dataArr *arr,DLQ *q, char* filename, node *self){
	(void) q;
	(void) filename;
        FILE *hashFile;
        hashFile = fopen(filename, "r");

        char *line = NULL;
        size_t len = 0;
        ssize_t nread;

        if(hashFile == NULL){
                fprintf(stderr, "File not found\n");
        }else{
                // Read in hash line by line, adding to DLQ
		// For each entry call getData to check if you have it
		// 	Set status appropriately.
		// 	If you don't have it, send request to get it
	
                while ((nread = getline(&line, &len, hashFile)) != -1) {
                        //printf("Retrieved line of length %zu:\n", nread);
                        fwrite(line, nread, 1, stdout);;
			int status = 1;
			//fprintf(stderr, "Trying to print line: %s\n", line);
                        if ((line)[nread - 1] == '\n') {
                                (line)[nread - 1] = '\0';
                        }
                        //fprintf(stderr, "Trying to print line after edit: %s\n", line);

                        dataPair* pair = getData(arr, line);
                        if(pair == NULL){
                                status = 0;
				com req;
                                req.type = htonl(1);
                                req.stat = htonl(0);
                                req.sourcePort = htonl(self->port);
                                memcpy(req.sourceIP, self->ipAdd, 16);
				memcpy(req.reqHash, line, 41);
                                //fprintf(stderr, "About to send request\n");
                                printNode(self);
                                pass(sizeof(req), (char *)&req, self->ipSucc, self->portSucc); 
                        }
                        insertDLQ(q, line, status);
                }

                free(line);
                fclose(hashFile);
		
        }

        //checkDLQ(arr, q);

}

void writeDL(dataArr *arr, DLQ *q){
        //fprintf(stderr, "Starting to write download");
        char* name = "DLResult";
        FILE *result = fopen(name, "w+");
        dataPair *pair;
        if(result == NULL){
                fprintf(stderr, "Unable to write DL\n");
        }else{
                for(size_t i = 0; i < q->used; i++){
                        pair = getData(arr, q->entries[i]->hash);
                        if(pair == NULL){
                                fprintf(stderr, "Missing required data\n%s\n", q->entries[i]->hash);
                                exit(1);
                        }else{
                                fwrite(pair->data, sizeof(char), pair->len, result);
                        }
                }
                //fprintf(stderr, "Finished downloading\n");
                fclose(result);
        }
}

void insertDLQ(DLQ *q, char *hash, int status){
        if(q == NULL || hash == NULL){
                fprintf(stderr, "Can't insert null into DLQ\n");
                exit(1);
        }

        if(q->max == q->used){
                growDLQ(q);
        }

	for(size_t i = 0; i < q->max; i++){
                if(q->entries[i] == NULL){
                        q->entries[i] = initQE(hash);
                        q->entries[i]->status = status;
                        break;
                }
        }
        q->used++;
}


void freeDLQ(DLQ *q){
        for (size_t i = 0; i < q->used; i++){
                free(q->entries[i]->hash);
                free(q->entries[i]);
        }
        free(q->entries);
	free(q);

}
