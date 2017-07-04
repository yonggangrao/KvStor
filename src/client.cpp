#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "util.cpp"

#define MAX_LINE 4096
#define DEFAULT_PORT 8000
#define DEFAULT_SERVER_IP "127.0.0.1"
using namespace std;

int main(int argc, char **argv)
{

	int socket_fd;
	struct sockaddr_in server_addr;
	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		cout << "Creat a  socket error: ";
		cout << strerror(errno) << " (errno:" << errno << ")" << endl;
		exit(0);
	}
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(DEFAULT_PORT);
	if (inet_pton(AF_INET, DEFAULT_SERVER_IP, &server_addr.sin_addr) <= 0) 
	{
		cout << "get address error: " ;
		cout << strerror(errno) << " (errno:" << errno << ")" << endl;
		exit(0);
	}

	int on = 1;
	if ((setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0) 
	{
		perror("setsockopt error\n");
		exit(0);
	}
	
	if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) 
	{
		cout << "connect error: " << strerror(errno);
		cout << " (errno:" << errno << ")" << endl;
		exit(0);
	}

	cout << endl << endl;
	cout << "-------KvStor Usage:-------";
	cout << endl << endl;
	cout << "command\t\tfunction\t\texample" << endl;
	cout << "set \t\tset key's value\t\teg.`set name Tom`" << endl;
	cout << "get \t\tget key's value\t\teg.`get name`" << endl;
	cout << "del \t\tdelete the key\t\teg.`del name`" << endl;
	cout << "exit \t\texit KvStor\t\teg.`exit`" << endl;
	cout << endl;

	char sendline[MAX_LINE], recvline[MAX_LINE];
	char buff[MAX_LINE];
    string op, input;
	while (true) 
	{
        cout << "KvStor> ";

        std::getline(cin, input);
        util::string s;
        input = s.trim(input);

        if ( !input.size() )
            continue;

        vector<string> v;

        s.split(input, " ", v);

        string op = v[0];
        if (op == "set" || op == "get" ||op == "del" || op == "exit")
        {
            if(op == "set")
            {
                if(v.size() != 3)
                {
                    cout << "bad command.." << endl;
                    continue;
                }
            }
            else if (op == "exit")
            {
                if(v.size() != 1)
                {
                    cout << "bad command.." << endl;
                    continue;
                }
            }
            else
            {
                if(v.size() != 2)
                {
                    cout << "bad command.." << endl;
                    continue;
                }
            }
            strcpy(sendline, input.c_str());

            if (send(socket_fd, sendline, strlen(sendline), 0) < 0) 
            {
                cout << "send messege to server error: " ;
                cout << strerror(errno) << " (errno:" << errno << ")" << endl;
                exit(0);
            }
            int len;
            if ((len = recv(socket_fd, buff, MAX_LINE, 0)) < 0) 
            {
                perror("receive from server error\n");
                exit(0);
            }
            if (op == "exit")
            {
                close(socket_fd);
                break;
            }
            cout << buff << endl;
            
        }
        
        else
        {
            cout << "command not found.." << endl;
            continue;
        }
	}
	
	return 0;
}
