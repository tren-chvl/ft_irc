#include "../includes/irc.hpp"


void Server::takePrivmsg(Client &client, const std::string &arg)
{
	std::string target;
	std::string msg;
	std::string full;
	size_t space;
	int targetFd;

	if (arg.empty())
		return (sendError(client, "411", ":No recipient given"));
	space = arg.find(' ');
	if (space == std::string::npos)
		return (sendError(client, "412", ":No text to send"));
	target = arg.substr(0, space);
	msg = arg.substr(space + 1);
	if (!msg.empty() && msg[0] == ':')
		msg.erase(0, 1);
	if (msg.empty())
		return (sendError(client, "412", ":No text to send"));
	if (!target.empty() && target[0] == '#')
	{
		std::map<std::string, Channel>::iterator it = channels.find(target);
		if (it == channels.end())
			return (sendError(client, "403", target + " :No such channel"));
		Channel &chan = it->second;
		if (!chan.isMember(client.getFd()))
			return (sendError(client, "404", target + " :Cannot send to channel"));
		full = ":" + client.getNickname() + " PRIVMSG " + target + " :" + msg + "\r\n";
		for (std::set<int>::const_iterator it2 = chan.getMember().begin(); it2 != chan.getMember().end(); ++it2)
		{
			if (*it2 != client.getFd())
				send(*it2, full.c_str(), full.size(), 0);
		}
		return ;
	}
	targetFd = -1;
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second.getNickname() == target)
		{
			targetFd = it->first;
			break ;
		}
	}
	if (targetFd == -1)
		return (sendError(client, "401", target + " :No such nick"));
	full = ":" + client.getNickname() + " PRIVMSG " + target + " :" + msg + "\r\n";
	send(targetFd, full.c_str(), full.size(), 0);
}
