#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include <semaphore.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0

typedef struct neighbor{

} clientList;

typedef struct node {
	int ipAdd;
	int port;

        int ipAddSucc;
	int portSucc;

	int ipAddPred;
	int portPred;
}

void loop();
void* getInput();
int hex2int(char ch);
int greaterThanHash(char *hash1, char *hash2);
pthread_mutex_t modTableState;


int main(int argc, char *argv[])
{
	(void) argc;
        (void) argv;
        int initialized = FALSE;
	if(!initialized++){
		int success = pthread_mutex_init(&modTableState, NULL);
		if(success){
                        fprintf(stderr, "Failed to create mutex\n");
			exit(1);
                }
        }
	
	pthread_t inputThread;
	if(pthread_create(&inputThread, NULL, getInput, NULL) < 0){
		fprintf(stderr, "Failed to create thread \n");
        }

	network();
}


//pthread_mutex_lock(&modTableState);
//pthread_mutex_unlock(&modTableState);
//pthread_join(inputThread, NULL);

void network()
{
	int i = 0;
        for(;;){
		//fprintf(stderr, "In the loop %i\n", i);
		i++;
		sleep(1);
        }
}

int hex2int(char ch)
{
        if (ch >= '0' && ch <= '9')
                return ch - '0';
        if (ch >= 'A' && ch <= 'F')
                return ch - 'A' + 10;
        if (ch >= 'a' && ch <= 'f')
                return ch - 'a' + 10;
        return -1;
}

int greaterThanHash(char *hash1, char *hash2)
{
	int length = 40;
	//int greater = FALSE;
	for(int i = 0; i < length; i++){
                char char1 = hash1[i];
		char char2 = hash2[i];
		int num1 = hex2int(char1); 
                int num2 = hex2int(char2);
		fprintf(stderr, "(%c %i),(%c %i)\n", char1, num1, char2, num2);
		if (num1 > num2){
                        return TRUE;
                }else if(num1 == num2){
                        continue;
                }else{
                        return FALSE;
                }
        }
	return FALSE; //looped through entire hash and was equal. 
}

void* getInput()
{
	char str[100];
	//char *str = NULL;
	char t;
        for(;;){
		fflush(stdin);
		fprintf(stdout, "Enter a command:\nupload: ['u' filename] | "
				"search: ['s' filename.fh] | force ft update"
				"['t']\n");
		scanf(" %c %s", &t, (char *)&str);
		
		switch(t) {
                case 'u' :
                        fprintf(stderr, "Upload file:%s\n", str);
                        break;
                case 'd' :
                        fprintf(stderr, "Download file:%s\n", str);
			break;
		case 's' : 
			fprintf(stderr, "Search for file:%s\n", str);
			break;
		case 't' :
			fprintf(stderr, "Force update\n");
                        break;
                default :
			fprintf(stderr, "Invalid command type\n");
			//fprintf(stderr, "%c, %s\n", t, str);
                }

        }
	fprintf(stderr, "Broke outside of loop\n");
	exit(1);
}
