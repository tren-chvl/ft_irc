#include "../../includes/irc.hpp"

void Server::takeJoin(Client &client, const std::string &arg)
{
	if (!client.regist)
		return (sendError(client, "451", ":You have not registered"));
	if (arg.empty())
		return (sendError(client, "461", "JOIN :Not enough parameters"));
	std::stringstream ss(arg);
	std::string chanName, key;
	ss >> chanName >> key;
	if (chanName.empty())
		return (sendError(client, "461", "JOIN :Not enough parameters"));
	if (chanName[0] != '#')
		chanName = "#" + chanName;
	std::map<std::string, Channel>::iterator it = channels.find(chanName);
	if (it == channels.end())
	{
		channels.insert(std::make_pair(chanName, Channel(chanName)));
		it = channels.find(chanName);
	}
	Channel &chan = it->second;
	if (chan.isMember(client.getFd()))
		return;
	if (chan.hasMode('l') && chan.getMember().size() >= static_cast<size_t>(chan.getLimit()))
		return (sendError(client, "471", chanName + " :Cannot join channel (+l)"));
	if (chan.hasMode('k') && key != chan.getKey())
		return (sendError(client, "475", chanName + " :Cannot join channel (+k)"));
	if (chan.hasMode('i') && !chan.isInvited(client.getFd()))
		return (sendError(client, "473", chanName + " :Cannot join channel (+i)"));
	chan.addMember(client.getFd());
	std::string joinmsg = ":" + client.getNickname() + " JOIN " + chanName + "\r\n";
	for (std::set<int>::const_iterator it2 = chan.getMember().begin(); it2 != chan.getMember().end(); ++it2)
		send(*it2, joinmsg.c_str(), joinmsg.size(), 0);
	std::string names = "= " + chanName + " :";
	for (std::set<int>::const_iterator it2 = chan.getMember().begin(); it2 != chan.getMember().end(); ++it2)
		names += clients[*it2].getNickname() + " ";
	names += "\r\n";
	send(client.getFd(), names.c_str(), names.size(), 0);
	std::cout << "FD " << client.getFd() << " joined " << chanName << std::endl;
}
