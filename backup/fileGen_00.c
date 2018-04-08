#include <stdio.h>
#include <stdlib.h>

static const int CHUNKSIZE = 400; // bytes

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

int main (int argc, char *argv[]){

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

void printDataArr(dataArr *arr){
        fprintf(stderr,"Not yet implemented\n");
}


void inputFile(char* filename){


}

