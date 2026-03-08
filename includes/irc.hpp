#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <iostream>
#include <vector>
#include <poll.h>
#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

class Server 
{
public:
	Server(int port);
	~Server();
	void run();

private:
	int	port;
	int	Fd;
	std::vector<pollfd> pollFds;

	void initSocket();
};

#endif