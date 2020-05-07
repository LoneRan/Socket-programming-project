*Code File*
I have wrote the code files including aws.c, client.c, serverA.c and serverB.c, all written on language C. 
In client.c, client build TCP connection with AWS server and send the map id, start vertex and file size to AWS server. 
In aws.c, I connect AWS server and client using TCP as well as  AWS server and server A/B using UDP, receiving the information from client and exchange with server A and server B to do the calculation, finally send all the calculated information back to client. All the information exchanged will be displayed. 
In serverA.c, what it mainly does is to read the map.txt and extract the information from it and build a list of matrix to store it, calculating the shortest path from start vertex to all other vertexes. 
And serverB.c use the shortest distance, propagation speed and transmission speed it received to calculate the propagation delay, transmission delay and total delay.

*Format of Messages*
All the messages printed on the terminal follow the format of instruction
For start vertex, file size, shortest distance to specific node, propagation speed, transmission speed, set the format as integer; for calculated transmission delay, propagation delay and total delay, set the format as float, and print with two decimals. The information from the map.txt is stored in two-dimension-array named MapMatrix with dynamic size, the map id is then transformed from characters to integers according to ASCII.

The map file should be named as “map.txt”. And for now, the propagation speed and transmission speed has to be integers. The vertex number need not to be sequential but the number of vertex has to be under 10.

Client:
e.g.:
./client B 2 453435
The client is up and running. 
The client has sent query to AWS using TCP over port 34672; start vertex 2; map B; file size 453435 
The client has received results from AWS:
------------------------------------------------
Destination    Min Length    Tt    Tp     Delay
------------------------------------------------
0                  11       11.34   0.00   11.34
1                  10       11.34   0.00   11.34
2                  0        0.00   0.00    0.00
3                  14       11.34   0.00   11.34
4                  05       11.34   0.00   11.34
5                  06       11.34   0.00   11.34
------------------------------------------------

AWS server:
The AWS is up and running. 
The AWS has received map ID B, start vertex 2 and file size 453435 from the client using TCP over port 50830.
The AWS has sent map ID and starting vertex to server A using UDP over port 21672.
The AWS has receieved shortest path from serverA:
---------------------------
Destination   Min Length
---------------------------
0           11
1           10
2           0
3           14
4           5
5           6
---------------------------
The AWS has sent path length, propagation speed and transmission speed to server B using UDP over port 22672
The AWS has received delays from server B:
---------------------------------------------
Destination          Tt       Tp        Delay
---------------------------------------------
0                  11.34       0.00       11.34
1                  11.34       0.00       11.34
2                  0.00        0.00       0.00
3                  11.34       0.00       11.34
4                  11.34       0.00       11.34
5                  11.34       0.00       11.34
---------------------------------------------
The AWS has sent calculated delay to client using TCP over port 50830.

Server A:
The Server A is up and running using UDP on port 21672.
The Server A has constructed a list of 2 maps:
-----------------------------------------------------------------------
Map ID                  Num Vertices                 Num Edges
A                          6                            8
B                          6                            6
----------------------------------------------------------------------
The server A has recieved input for finding shortest paths: starting vertex 2 of map B
The Server A has identified the following shortest paths:
---------------------------
Destination   Min Length
---------------------------
0           11
1           10
2           0
3           14
4           5
5           6
---------------------------
The Server A has sent shortest paths to AWS.

Server B:
The Server B is up and running using UDP on port 22672.
The Server B has recieved data for calculation:
* Propagation speed: 3000 km/s;
* Transmission speed: 5000 Bytes/s;
* filesize: 453435 bits;
* path length for destination 0: 11
* path length for destination 1: 10
* path length for destination 2: 0
* path length for destination 3: 14
* path length for destination 4: 5
* path length for destination 5: 6
The Server B has finished the calculation of the delays:
------------------------
Destination        Delay
0                     11.34
1                     11.34
2                     11.34
3                     11.34
4                     11.34
5                     11.34
------------------------
The Server B has finished sending the output to AWS.



*Reused Code*
In client.c and aws.c, set up TCP connection by copying code from Client-server Background of Beej-Guide-Network-Programming. 
In aws.c, serverA.c and serverB.c, set up UDP sockets by copying from the same chapter from Beej-Guide-Network-Programming.
Dijkstra code reference from www.cnblogs.com/skywang12345/

*Idiosyncrasy*
When the propagation speed or transmission speed contains decimal, my project will only read the integer part, which causes inaccuracy.
The map ID are better to named sequentially like (A B C D ...), I think unsequential map ID might cause some problem.
In my project, I set the maximum number of rows of map.txt to 100.
