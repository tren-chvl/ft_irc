#include "../../includes/irc.hpp"

void Server::takePart(Client &client, const std::string &arg)
{
	std::string chanName;
	std::string msg;

	if (!client.regist)
		return (sendError(client, "451", ":You have not registered"));
	if (arg.empty())
		return (sendError(client, "461", "PART :Not enough parameters"));
	chanName = arg;
	if (!chanName.empty() && chanName[0] == ':')
		chanName.erase(0, 1);
	std::map<std::string, Channel>::iterator it = channels.find(chanName);
	if (it == channels.end())
		return (sendError(client, "403", chanName + " :No such channel"));
	Channel &chan = it->second;
	if (!chan.isMember(client.getFd()))
		return (sendError(client, "442", chanName + " :You're not on that channel"));
	msg = ":" + client.getNickname() + " PART " + chanName + "\r\n";
	for (std::set<int>::const_iterator it2 = chan.getMember().begin(); it2 != chan.getMember().end(); ++it2)
		send(*it2, msg.c_str(), msg.size(), 0);
	chan.removeMember(client.getFd());
	if (chan.getMember().empty())
		channels.erase(chanName);
	std::cout << "FD " << client.getFd() << " left " << chanName << std::endl;
}

