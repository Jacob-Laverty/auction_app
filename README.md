#Auction App

#Compile and run 
CD into src. Compile server with make server. Compile client
with make client.  Run server with ./server. The ports for client and seller
are defaulted to 8888 and 7777 respectively. To run a seller run ./client -p
7777 -h 127.0.0.1 to run a buyer run ./client -p 8888 -h 127.0.0.1.

If you want to use non default vaules for the server there are switches to
change the default vaules. ./server -s 1111 -b 2222 will set the server port to
1111 and the buyer port to 2222. Also run on zeus please.

#Design Overview

The design is relatively straightforward just a server and client. The client
is pretty bare and just acts as a display and a forward to the server. The
server has a linked list of all of the items currently on the server. It also
has an array of threads. New threads are created for every client and each
thread will handle the requests for their own connection. 

###Not working.

The extra credit was no implemented and winning bids aren't pushed to the
buyer. But they are pushed to the seller.
