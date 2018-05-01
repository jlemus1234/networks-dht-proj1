# networks-dht-proj1

Jose Lemus

Compile: gcc -fileGen.c hashing.c node.c download.c -lnsl -lcrypto -pthread 

To run the program as a single node, starting a new network
./a.out [port]

To run the program and join an existing network
./a.out [port] [hostname of node in network] [port of node in network]


Commands:
Upload a file: 		u [filename]
Download a file: 	d [filename.fh]
Scan network: 		s [filename]
Print all data chunks: 	p c
Print node ID, successor, and predecessor: n a


