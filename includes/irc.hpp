#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <poll.h>
#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "Client.hpp"
#include <map>


class Server 
{
	private:
		int	port;
		int	Fd;
		std::vector<pollfd> pollFds;
		std::map<int, Client> clients;

	void initSocket();
	void acceptClient(); 
	void Client_msg(int clienFd);
	void remove_Client(int clientFd);
	void client_to_buf(Client &client);
	void parse_command(Client &client, const std::string &cmd);
	void takePass(Client &client, const std::string &arg);
	void takeNick(Client &client, const std::string &arg);
	void takeUser(Client &client, const std::string &arg);
	void regist_Client(Client &client);
	void takePing(Client &client, const std::string &arg);

	public:
		Server(int port);
		~Server();
		void run();
};

#endif