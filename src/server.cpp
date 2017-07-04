#include <unordered_map>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <iostream>
#include "skip_list.cpp"
#include "KvStor.cpp"

using namespace KvStor;

using namespace std;
#define DEFAULT_PORT  8000
#define MAX_LINE 4096
#define LEN (KEY_SIZE+VALUE_SIZE+2)
static mutex put_mutex, get_mutex;
static void handle_client_requests(int connected_fd);

kvstor* kv;
int main(int argc, char* argv[])
{
	kv = new kvstor();
	if( !kv->create_kvstor())
	{
		return -1;
	}
	int socket_fd, connected_fd;

	
	char buff[MAX_LINE];
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int sin_addr_size = sizeof(server_addr);
	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{
		cout << "Creat a socket error: " << strerror(errno) << " (errno:" << errno << ")" << endl;
		exit(0);
	}
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;

	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
	server_addr.sin_port = htons(DEFAULT_PORT); 
	int on = 1;
	if ((setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0) 
	{
		perror("setsockopt error\n");
		exit(0);
	}
	//bind
	if (bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) 
	{
		cout << "bind  socket error: " << strerror(errno) << " (errno:" << errno << ")" << endl;
		exit(0);
	}
	if (listen(socket_fd, 10) == -1) 
	{
		cout << "listen  socket error: " << strerror(errno) << " (errno:" << errno << ")" << endl;
		exit(0);
	}
	cout << endl << endl;
	cout << "-------Waiting for clients's request-------";
	cout << endl << endl;

	while(true) 
	{
		if ((connected_fd = 
			accept(socket_fd, (struct sockaddr*)&client_addr, (socklen_t*)&sin_addr_size)) == -1) 
		{
			cout << "accept  socket error: " << strerror(errno) << " (errno:" << errno << ")" << endl;
			continue;
		}
		cout << "client " << connected_fd - 3 << " is connecting." << endl; // 0~2 is file_thread; 3 is main_thread;4 is the first client
		cout << "IP: " << inet_ntoa(client_addr.sin_addr) << "  " << "Port: " << htons(client_addr.sin_port) << endl;
		thread client(handle_client_requests, connected_fd); // new a thread to handle current client's requests
		client.detach(); // detach this client thread, or you must wait it ( client.join() ).
	}
	close(socket_fd);
	cout << "server finish.." << endl;
	return 0;
}

static void handle_client_requests(int connected_fd) 
{
	cout << "client " << connected_fd - 3 << " connected success" << endl; // 0~3 thread are default; 4 is the first client 
	char buff[MAX_LINE];
	memset(buff, 0, MAX_LINE);
	buff[0] = '\0';
	while (true) 
	{
		int len = recv(connected_fd, buff, MAX_LINE, 0);  // 0 is a flag
		if (len == 0)
			continue;

		cout << buff << endl;

		keyType key;
		valueType value;
		valueType *val;
		string op;
		string input(buff);

 		util::string s;
        input = s.trim(input);

		if ( !input.size() )
			buff[0] = '\0';

		vector<string> v;
		s.split(input, " ", v);

		op = v[0];
		if (op == "set")
		{
			if(v.size() != 3)
            {
            	strcpy(buff, "bad command..");
            }
            else
            {
            	key = v[1];
				value = v[2];
				put_mutex.lock();
				get_mutex.lock();
				if (kv->set(key, value))
            		strcpy(buff, "OK!");
            	else 
            		strcpy(buff, "error..");
            	get_mutex.unlock();
				put_mutex.unlock();

            }
		}
		else if (op == "get")
		{
			if(v.size() != 2)
            {
            	strcpy(buff, "bad command..");
            }
            else
            {
				key = v[1];
				put_mutex.lock();
				val = kv->get(key);
				put_mutex.unlock();
				if (val)
            		strcpy(buff, (*val).c_str());
            	else
					buff[0] = '\0';
            }
		}
		else if (op == "del")
		{
			if(v.size() != 2)
            {
            	strcpy(buff, "bad command..");
            }
            else
            {

				key = v[1];
				put_mutex.lock();
				get_mutex.lock();
				if (kv->del(key))
            		strcpy(buff, "OK!");
            	else 
            		strcpy(buff, "error..");
            	get_mutex.unlock();
				put_mutex.unlock();
            }
		}
		else if (op == "exit")
		{
			if(v.size() != 1)
            {
            	strcpy(buff, "bad command..");
            }
            else
            {
				put_mutex.lock();
				kv->destory_kvstor();
				put_mutex.unlock();
            }
		}
		else
		{
           	strcpy(buff, "bad command..");
		}

		if(send(connected_fd, buff, MAX_LINE, 0) == -1) 
		{
            perror("send error");
		}
		memset(buff, 0, MAX_LINE);

	}
	close(connected_fd);
}
