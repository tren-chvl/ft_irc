#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <poll.h>
#include <set>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <map>
#include "Client.hpp"
#include "channel.hpp"


class Server 
{
	private:
		int	port;
		int	Fd;
		std::string password;
		std::string serverName;
		std::vector<pollfd> pollFds;
		std::map<int, Client> clients;
		std::map<std::string, Channel> channels;

	void initSocket();
	void acceptClient(); 
	void Client_msg(int clienFd);
	void remove_Client(int clientFd);
	void client_to_buf(Client &client);
	void parse_command(Client &client, const std::string &cmd);
	bool parseModeArguments(const std::string &arg, std::string &chanName, std::string &modes, std::vector<std::string> &params);
	void applySingleMode(Channel &chan, char sign, char mode, const std::string &param);
	void applyAllModes(Channel &chan, const std::string &modes, const std::vector<std::string> &params);
	void broadcastModeChange(Channel &chan, const std::string &msg);
	void sendError(Client &client, const std::string &code , const std::string &msg);
	int getFdByNick(const std::string &nick);
	void regist_Client(Client &client);

	void takePass(Client &client, const std::string &arg);
	void takeNick(Client &client, const std::string &arg);
	void takeUser(Client &client, const std::string &arg);
	void takePing(Client &client, const std::string &arg);
	void takeQuit(Client &client, const std::string &arg);
	void takeJoin(Client &client, const std::string &arg);
	void takePrivmsg(Client &client, const std::string &arg);
	void takePart(Client &Client, const std::string &arg);
	void takeTopic(Client &client, const std::string &arg);
	void takeKick(Client &client, const std::string &arg);
	void takeMode(Client &client , const std::string &arg);
	void takeInvite(Client &client, const std::string &arg);
	public:
		Server(int port, const std::string &password);
		~Server();
		void run();
};

#endif
