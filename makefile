default:
	gcc -g -o server src/server/main.c src/send.c src/network_server.c
	gcc -g -o client src/client/main.c src/send.c src/network_client.c
	
client:
	gcc -g -o client src/client/main.c src/send.c src/network_client.c
server:
	gcc -g -o server src/server/main.c src/send.c src/network_server.c
clean:
	rm client server
