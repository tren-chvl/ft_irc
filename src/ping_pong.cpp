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