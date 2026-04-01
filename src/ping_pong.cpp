#include "../includes/irc.hpp"

void Server::takePing(Client &client, const std::string &arg)
{
	std::string clean = arg;
	if (!clean.empty() && clean[0] == ':')
		clean = clean.substr(1);
	if (!clean.empty() && clean[clean.size() - 1] == '\r')
		clean.erase(clean.size() - 1);
	std::string reply = "PONG :" + clean + "\r\n";
	send(client.getFd(), reply.c_str(), reply.size(), 0);
}

void Server::takeQuit(Client &client, const std::string &arg)
{
	std::string msg = arg.empty() ? "Client Quit" : arg;
	std::cout << "Client " << client.getFd() << " QUIT ("  << msg << ")" << std::endl;
	remove_Client(client.getFd());
}

void Server::takePart(Client &client, const std::string &arg)
{
	if (!client.regist)
		return;
	if (arg.empty())
		return;
	std::string chanName = arg;
	if (!chanName.empty() && chanName[0] == ':')
		chanName.erase(0, 1);
	if (!channels.count(chanName))
		return;
	Channel &chan = channels.find(chanName)->second;
	if (!chan.isMember(client.getFd()))
		return ;
	std::string msg = ":" + client.getNickname() + " PART " + chanName + "\r\n";
	for (std::set<int>::const_iterator it = chan.getMember().begin();it != chan.getMember().end();++it)
		send(*it, msg.c_str(), msg.size(), 0);
	chan.removeMember(client.getFd());
	if (chan.getMember().empty())
		channels.erase(chanName);
	std::cout << "FD " << client.getFd() << " left " << chanName << std::endl;
}

void Server::takeInvite(Client &client, const std::string &arg)
{
	if (!client.regist)
		return (sendError(client, "451", ":You have not registered"));
	if (arg.empty())
		return (sendError(client, "461", "INVITE :Not enough parameters"));
	std::stringstream ss(arg);
	std::string chanName;
	std::string nick;
	std::string reply;
	std::string msg;
	ss >> nick >> chanName;
	if (nick.empty() || chanName.empty())
		return (sendError(client, "461", "INVITE :Not enough parameters"));
	std::map<std::string, Channel>::iterator it = channels.find(chanName);
	if (it == channels.end())
		return (sendError(client, "403", chanName + " :No such channel"));
	Channel &chan = it->second;
	if (!chan.isMember(client.getFd()))
		return (sendError(client, "442", chanName + " :You're not on that channel"));
	if (!chan.isOperator(client.getFd()))
		return (sendError(client, "482", chanName + " :You're not channel operator"));
	int targetFd = -1;
	for (std::map<int, Client>::iterator itc = clients.begin(); itc != clients.end(); ++itc)
	{
		if (itc->second.getNickname() == nick)
		{
			targetFd = itc->first;
			break ;
		}
	}
	if (targetFd == -1)
		return (sendError(client, "401", nick + " :No such nick"));
	if (chan.isMember(targetFd))
		return (sendError(client, "443", nick + " " + chanName + " :is already on channel"));
	chan.addInvited(targetFd);
	reply = ":server 341 " + client.getNickname() + " " + nick + " " + chanName + "\r\n";
	send(client.getFd(), reply.c_str(), reply.size(), 0);
	msg = ":" + client.getNickname() + " INVITE " + nick + " " + chanName + "\r\n";
	send(targetFd, msg.c_str(), msg.size(), 0);
}


