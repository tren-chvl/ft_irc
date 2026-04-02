#include "../../includes/irc.hpp"

void Server::takeTopic(Client &client, const std::string &arg)
{
	if (!client.regist)
		return (sendError(client, "451", ":You have not registered"));
	size_t space = arg.find(' ');
	std::string chanName = arg.substr(0, space);
	if (chanName.empty())
		return (sendError(client, "461", "TOPIC :Not enough parameters"));
	std::map<std::string, Channel>::iterator it = channels.find(chanName);
	if (it == channels.end())
		return (sendError(client, "403", chanName + " :No such channel"));
	Channel &chan = it->second;
	if (!chan.isMember(client.getFd()))
		return (sendError(client, "442", chanName + " :You're not on that channel"));
	if (space == std::string::npos)
	{
		if (chan.getTopic().empty())
		{
			std::string reply = ":server 331 " + client.getNickname() + " " + chanName + " :No topic is set\r\n";
			send(client.getFd(), reply.c_str(), reply.size(), 0);
		}
		else
		{
			std::string reply = ":server 332 " + client.getNickname() + " " + chanName + " :" + chan.getTopic() + "\r\n";
			send(client.getFd(), reply.c_str(), reply.size(), 0);
		}
		return ;
	}
	if (chan.hasMode('t') && !chan.isOperator(client.getFd()))
		return (sendError(client, "482", chanName + " :You're not channel operator"));
	std::string newTopic = arg.substr(space + 1);
	if (!newTopic.empty() && newTopic[0] == ':')
		newTopic.erase(0, 1);
	chan.setTopic(newTopic);
	std::string msg = ":" + client.getNickname() + " TOPIC " + chanName + " :" + newTopic + "\r\n";
	for (std::set<int>::const_iterator it2 = chan.getMember().begin(); it2 != chan.getMember().end(); ++it2)
		send(*it2, msg.c_str(), msg.size(), 0);
}
