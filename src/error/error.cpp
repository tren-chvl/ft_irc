#include "../../includes/irc.hpp"

void Server::sendError(Client &client, const std::string &code , const std::string &msg)
{
	std::string error = "::" + serverName + " " + code + " " + client.getNickname() + " "  + msg + "\r\n";
	send(client.getFd(), error.c_str(), error.size(), 0);  
}

