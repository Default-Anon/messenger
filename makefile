default:
	gcc -g -o server src/server/main.c src/send.c src/network_server.c
	gcc -g -o client src/client/main.c src/send.c src/network_client.c
	
client:
	gcc -g -o client src/client/main.c src/send.c src/network_client.c
server:
	gcc -g -o server src/server/main.c src/send.c src/network_server.c
servergui:
	gcc -o server src/server/main.c src/send.c src/network_server.c src/gui.c -fpic -DRAYGUI_IMPLEMENTATION -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
release:
	gcc -o server src/server/main.c src/send.c src/network_server.c -Wall -O3
	gcc -o client src/client/main.c src/send.c src/network_client.c -Wall -O3
clean:
	rm client server output
