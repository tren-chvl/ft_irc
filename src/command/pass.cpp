#include "../../includes/irc.hpp"


void Server::takePass(Client &client, const std::string &arg)
{
	if (client.has_Pass)
		return;
	if (arg.empty())
	{
		std::cout << "PASS missing for FD " << client.getFd() << std::endl;
		remove_Client(client.getFd());
		return;
	}
	if (arg != password)
	{
		std::cout << "Wrong PASS for FD " << client.getFd() << std::endl;
		remove_Client(client.getFd());
		return;
	}
	client.has_Pass = true;
	std::cout << "PASS OK for FD " << client.getFd() << std::endl;
}