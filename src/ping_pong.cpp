#include "../includes/irc.hpp"

void Server::takePing(Client &client, const std::string &arg)
{
	std::string clean = arg;
	if (!clean.empty() && clean[0] == ':')
		clean = clean.substr(1);
	if (!clean.empty() && clean[clean.size() - 1] == '\r')
		clean.erase(clean.size() - 1);
	std::string reply = "PONG :" + clean + "\r\n";
	send(client.getFd(), reply.c_str(), reply.size(), 0);
}



