cc = g++ -g -O0 -std=c++11 -pthread -Wl,--no-as-needed -g -D_REENTRANT
server_objects = server.o 
server: $(server_objects)
	$(cc) $(server_objects) -o server

server.o: server.cpp skip_list.cpp KvStor.cpp
	$(cc) -c server.cpp

client: client.o
	$(cc) client.o -o client
client.o: client.cpp util.cpp
	$(cc) -c client.cpp

start_up: start_up.o
	$(cc) start_up.o -o start_up
start_up.o: start_up.cpp skip_list.hpp KvStor.hpp
	$(cc) -c start_up.cpp

clean:
	rm *.o server client start_up 
