send.c -> User enters IP & PATH & Send command, when Input is valid it sends data by using UDP. Create new file when data received from socket and write data into that file.


udpserver.c -> bind server properties and use sendto func to send data to client.
udpclient.c -> reads data from file and sends data from client to server. 
