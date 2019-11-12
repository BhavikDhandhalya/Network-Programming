Name : Bhavik Dhandhalya
ID : 2018H1030118P

Server file:
tcp_server_select_oddeven.c

Client file:
test_client.c

Flow of code:
1. server will wait for all client to connect
2. Then server will receive data from all clients
3. Based on client fd, server will send data

Steps:
1. Open 5 Terminal as shown in npEx7.png picture.
2. RUN all 5 command at a time with minimum delay.

3. for server
<./filename> <number of clients> <port>

--------------SERVER------------------
gcc tcp_server_select_oddeven.c -o tcp
./tcp 4 8000
--------------------------------------

4. for client
<./filename> <port> <message that client will send>

---------------CLIENT-----------------
gcc test_client.c -o test
./test 8000 Message
--------------------------------------