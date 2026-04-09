#include "../../includes/irc.hpp"

void Server::takeQuit(Client &client, const std::string &arg)
{
	std::string quitMsg = arg.empty() ? "Client Quit" : arg;
	std::string fullQuit;
	if (!quitMsg.empty() && quitMsg[0] == ':')
		quitMsg.erase(0, 1);
	fullQuit = ":" + client.getNickname() + " QUIT :" + quitMsg + "\r\n";
	for (std::map<std::string, Channel>::iterator it = channels.begin();it != channels.end();)
	{
		Channel &chan = it->second;
		if (chan.isMember(client.getFd()))
		{
			for (std::set<int>::const_iterator it2 = chan.getMember().begin(); it2 != chan.getMember().end(); ++it2)
			{
				if (*it2 != client.getFd())
					send(*it2, fullQuit.c_str(), fullQuit.size(), 0);
			}
			chan.removeMember(client.getFd());
			if (chan.getMember().empty())
			{
				std::map<std::string, Channel>::iterator toErase = it++;
				channels.erase(toErase);
				continue ;
			}
		}
		++it;
	}
	std::cout << "Client " << client.getFd() << " QUIT (" << quitMsg << ")" << std::endl;
	client.Disco_down();
}
