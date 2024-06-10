default:
	gcc -g -o server src/server/main.c src/send.c src/network.c
	gcc -g -o client src/client/main.c src/send.c src/network.c
	
client:
	gcc -g -o client src/client/main.c src/send.c src/network.c
server:
	gcc -g -o server src/server/main.c src/send.c src/network.c
clean:
	rm client server
