#include "../../includes/irc.hpp"

void Server::takePing(Client &client, const std::string &arg)
{
	std::string clean = arg;
	std::string reply;
	if (!clean.empty() && clean[0] == ':')
		clean.erase(0, 1);
	while (!clean.empty() && (clean[clean.size() - 1] == '\r' || clean[clean.size() - 1] == '\n'))
	{
		clean.erase(clean.size() - 1);
	}
	if (clean.empty())
		clean = "server";
	reply = "PONG :" + clean + "\r\n";
	send(client.getFd(), reply.c_str(), reply.size(), 0);
}
