client: communicate src/client.c include/client.h
	gcc -Wall -pthread -Iinclude -o client src/client.c

communicate: src/client_communication.c include/client.h
	gcc -Wall -pthread -Iinclude -o communicate src/client_communication.c

clean:
	rm client
	rm communicate
	rm -rf log
