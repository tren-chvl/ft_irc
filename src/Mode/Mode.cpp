#include "../../includes/irc.hpp"

void Server::takeMode(Client &client, const std::string &arg)
{
	char sign = 0;

	if (!client.regist)
		return ;
	std::stringstream ss(arg);
	std::string chanName;
	ss >> chanName;
	if (!channels.count(chanName))
		return ;
	Channel &chan = channels.find(chanName)->second;
	if (!chan.isOperator(client.getFd()))
	{
		std::string error = ":server 482 " + client.getNickname() + " " + chanName + " :You're not channel operator\r\n";
		send(client.getFd(), error.c_str(), error.size(), 0);
		return;
	}
	std::string Mode;
	ss >> Mode;
	if (mode.empty())
		return;
	std::string param;
	if (!mode.empty())
	{
		char c Mode[0];
		Mode.erase(0, 1);
		if (c == '-' || c == '+')
		{
			sign = c;
			continue;
		}
	}
	if (c == 'k' || c == 'l' || c == 'o')
	{
		ss >> param;
		if (param.empty())
			continue;
	}
	
}