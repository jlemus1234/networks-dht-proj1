#include <stdio.h>
#include <string.h>
#include "hashing.h"

#define num_entries 5
#define HASH_LEN 40
int max_index = num_entries - 1;


int pred;
int largest_node;
char *self;
char finger_table[num_entries][HASH_LEN + 1];
/*int sockfd;
struct sockaddr_in serveraddr;
/*
void init(int portno, str hostname, int hostport);
void init(int portno);
*/


int next_hop(char key[HASH_LEN + 1]);


int main()
{
	int num = 1;

	self = "0000000000000000000000000000000000000000\0";
	largest_node = 1;
	pred = 0;

	int split_val = 6;

	fprintf(stderr, "Making Table\n");
/*	for(int i = split_val; i < num_entries; i++) {
		finger_table[i] = num;
		num *= 2;
        }

	self = num;

	for(int i = 0; i < split_val; i++) {
		finger_table[i] = num;
		num *= 2;
                }*/

	memcpy( finger_table[0], "1234567890123456789012345678901234567890\0", HASH_LEN + 1); 

	memcpy( finger_table[1], "2234567890123456789012345678901234567890\0", HASH_LEN + 1); 

	memcpy( finger_table[2], "3234567890123456789012345678901234567890\0", HASH_LEN + 1); 

	memcpy( finger_table[3], "4234567890123456789012345678901234567890\0", HASH_LEN + 1); 

	memcpy( finger_table[4], "5234567890123456789012345678901234567890\0", HASH_LEN + 1); 


	for(int i = 0; i < num_entries; i++) {
		fprintf(stderr, "entry %d: %s\n", i, finger_table[i]);
        }
	fprintf(stderr, "Next hop for %s is %s\n",  "4000000000000000000000000000000000000000\0", finger_table[next_hop( "4000000000000000000000000000000000000000\0")]);

	
/*	if(argc == 2) {
		init(argv[1]);
        }
	else if(argc == 4) {
		init(strtol(argv[1]), argv[2], strtol(argv[3]));
        }
	else
                fprintf("Usage: "); /************************** add usage message ******************/
	return 0;
}
/*
void init(int portno) {
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sock_fd < 0)
		error("ERROR OPENING SOCKET");
	server 
}

void init(int portno, str hostname, int hostport) {
}
*/

int next_hop(char key[40]) {
	int index = 0;

	if (greaterThanHash(key,  self)) {
		while(greaterThanHash(key, finger_table[index + 1]) && index < max_index) {
			index++;
//			fprintf(stderr, "key was larger than %d\n", finger_table[index]);
			if (greaterThanHash(finger_table[index], finger_table[index + 1])) 
				break;
                }
		return index;
	}
	/* key < self */
		index = max_index;
		while(greaterThanHash(finger_table[index], key) && index > 1) {
			index--;
//			fprintf(stderr, "key was less than %d\n", finger_table[index]);
                }
		return index;
}
