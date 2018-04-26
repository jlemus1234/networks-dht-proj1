
#include <stdio.h>

#define num_entries 10
int max_index = num_entries - 1;

int pred;
int largest_node;
int self;
int finger_table[num_entries];
/*int sockfd;
struct sockaddr_in serveraddr;
/*
void init(int portno, str hostname, int hostport);
void init(int portno);
*/


int next_hop(int key);


int main()
{
	int num = 1;

	self = 0;
	largest_node = 1;
	pred = 0;

	int split_val = 6;

	fprintf(stderr, "Making Table\n");
	for(int i = split_val; i < num_entries; i++) {
		finger_table[i] = num;
		num *= 2;
        }

	self = num;

	for(int i = 0; i < split_val; i++) {
		finger_table[i] = num;
		num *= 2;
        }


	for(int i = 0; i < num_entries; i++) {
		fprintf(stderr, "entry %d: %d\n",i,finger_table[i]);
        }
	fprintf(stderr, "Next hop for %d is %d\n",2,next_hop(2));

	fprintf(stderr, "Next hop for %d is %d\n",6,next_hop(6));

	fprintf(stderr, "Next hop for %d is %d\n",12,next_hop(12));

	fprintf(stderr, "Next hop for %d is %d\n",30,next_hop(30));

	fprintf(stderr, "Next hop for %d is %d\n",127,next_hop(127));

	fprintf(stderr, "Next hop for %d is %d\n",129,next_hop(129));

	fprintf(stderr, "Next hop for %d is %d\n",511,next_hop(511));

	fprintf(stderr, "Next hop for %d is %d\n",514,next_hop(514));

	
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

int next_hop(int key) {
	int index = 0;

	if (key > self) {
		while(key > finger_table[index + 1] && index < max_index) {
			index++;
//			fprintf(stderr, "key was larger than %d\n", finger_table[index]);
			if (finger_table[index + 1] < finger_table[index]) 
				break;
                }
		return finger_table[index];
	}
	/* key < self */
		index = max_index;
		while(key < finger_table[index] && index > 1) {
			index--;
//			fprintf(stderr, "key was less than %d\n", finger_table[index]);
                }
		return finger_table[index];
}
