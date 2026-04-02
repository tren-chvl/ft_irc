#include "../../includes/irc.hpp"

void Server::takeKick(Client &client, const std::string &arg)
{
	std::string channelName;
	std::string targetNick;
	std::string reason;
	std::string msg;
	if (!client.regist)
		return (sendError(client, "451", ":You have not registered"));
	if (arg.empty())
		return (sendError(client, "461", "KICK :Not enough parameters"));
	size_t arg1 = arg.find(' ');
	if (arg1 == std::string::npos)
		return (sendError(client, "461", "KICK :Not enough parameters"));
	channelName = arg.substr(0, arg1);
	size_t arg2 = arg.find(' ', arg1 + 1);
	if (arg2 == std::string::npos)
		return (sendError(client, "461", "KICK :Not enough parameters"));
	targetNick = arg.substr(arg1 + 1, arg2 - (arg1 + 1));
	reason = arg.substr(arg2 + 1);
	if (!reason.empty() && reason[0] == ':')
		reason.erase(0, 1);
	std::map<std::string, Channel>::iterator it = channels.find(channelName);
	if (it == channels.end())
		return (sendError(client, "403", channelName + " :No such channel"));
	Channel &chan = it->second;
	if (!chan.isMember(client.getFd()))
		return (sendError(client, "442", channelName + " :You're not on that channel"));
	if (!chan.isOperator(client.getFd()))
		return (sendError(client, "482", channelName + " :You're not channel operator"));
	int targetFd = -1;
	for (std::map<int, Client>::iterator itc = clients.begin(); itc != clients.end(); ++itc)
	{
		if (itc->second.getNickname() == targetNick)
		{
			targetFd = itc->first;
			break;
		}
	}
	if (targetFd == -1)
		return sendError(client, "401", targetNick + " :No such nick");
	if (!chan.isMember(targetFd))
		return (sendError(client, "441", targetNick + " " + channelName + " :They aren't on that channel"));
	msg = ":" + client.getNickname() + " KICK " + channelName + " " + targetNick;
	if (!reason.empty())
		msg += " :" + reason;
	msg += "\r\n";
	for (std::set<int>::const_iterator it2 = chan.getMember().begin(); it2 != chan.getMember().end(); ++it2)
		send(*it2, msg.c_str(), msg.size(), 0);
	chan.removeMember(targetFd);
	if (chan.getMember().empty())
		channels.erase(channelName);
}