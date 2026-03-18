#include "../includes/irc.hpp"


void Server::takePrivmsg(Client &client, const std::string &arg)
{
	size_t space = arg.find(' ');
	if (space == std::string::npos)
		return;
	std::string target = arg.substr(0, space);
	std::string msg = arg.substr(space + 1);
	if (!msg.empty() && msg[0] == ':')
		msg.erase(0,1);
	if (msg.empty())
		return;
	if (target[0] == '#')
	{
		if (!channels.count(target))
			return;
		Channel &chan = channels.find(target)->second;
		std::string full = ":" + client.getNickname() + " PRIVMSG " + target + " :" + msg + "\r\n";
		for (std::set<int>::const_iterator it = chan.getMember().begin(); it != chan.getMember().end(); ++it)
		{
			if (*it != client.getFd())
				send(*it,full.c_str(), full.size(), 0);
		}
	}
	else
	{
		int targetFd = -1;
		for (std::map<int , Client>::iterator it = clients.begin(); it != clients.end();++it)
		{
		  if (it->second.getNickname() == target)
		  {
			targetFd = it->first;
			break;
		  } 
		}
		if (targetFd < 0)
			return ;
		std::string full = ":" + client.getNickname() + " PRIVMSG " + target + " :" + msg + "\r\n";
		send(targetFd, full.c_str(), full.size(), 0);
	}
}