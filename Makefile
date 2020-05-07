all:            client.c aws.c serverA.c serverB.c
		gcc -o client client.c 
		gcc -o aws aws.c 
		gcc -o serverA serverA.c 
		gcc -o serverB serverB.c 

serverA:        serverA.o
	        ./serverA

serverB:        serverB.o
	        ./serverB

aws:            aws.o
	        ./aws