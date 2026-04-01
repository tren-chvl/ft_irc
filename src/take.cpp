/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   take.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zcadinot <zcadinot@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 16:06:28 by zcadinot          #+#    #+#             */
/*   Updated: 2026/03/28 16:50:29 by zcadinot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/irc.hpp"

void Server::takePass(Client &client, const std::string &arg)
{
	if (client.has_Pass)
		return;
	if (arg.empty())
	{
		std::cout << "PASS missing for FD " << client.getFd() << std::endl;
		remove_Client(client.getFd());
		return;
	}
	if (arg != password)
	{
		std::cout << "Wrong PASS for FD " << client.getFd() << std::endl;
		remove_Client(client.getFd());
		return;
	}
	client.has_Pass = true;
	std::cout << "PASS OK for FD " << client.getFd() << std::endl;
}

void Server::takeNick(Client &client, const std::string &arg)
{
	if (!client.has_Pass)
		return (sendError(client, "451", ":You have not registered"));
	if (arg.empty())
		return (sendError(client, "431", ":No nickname given"));
	std::string nick = arg;
	for (size_t i = 0; i < nick.size(); i++)
	{
		if (!isalnum(nick[i]) && nick[i] != '-' && nick[i] != '_' &&
			nick[i] != '[' && nick[i] != ']' && nick[i] != '\\' &&
			nick[i] != '`')
		{
			return (sendError(client, "432", nick + " :Erroneous nickname"));
		}
	}
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second.getNickname() == nick)
			return (sendError(client, "433", nick + " :Nickname is already in use"));
	}
	client.setNickname(nick);
	client.has_Nick = true;
	std::cout << "NICK set: " << nick << " for FD " << client.getFd() << std::endl;
	if (client.has_Pass && client.has_Nick && client.has_User && !client.regist)
		regist_Client(client);
}
void Server::takeUser(Client &client, const std::string &arg)
{
	size_t pos;
	std::string username;
	std::string mode;
	std::string unused;
	std::string realname;
	if (!client.has_Pass)
		return (sendError(client, "451", ":You have not registered"));
	if (client.has_User)
		return (sendError(client, "462", ":You may not reregister"));
	std::stringstream ss(arg);
	ss >> username >> mode >> unused;
	if (username.empty() || mode.empty() || unused.empty())
		return (sendError(client, "461", "USER :Not enough parameters"));
	pos = arg.find(':');
	if (pos != std::string::npos)
		realname = arg.substr(pos + 1);
	else
		realname = "";
	client.setUsername(username);
	client.setRealname(realname);
	client.has_User = true;
	std::cout << "USER set: " << username << " (" << realname << ") for FD " << client.getFd() << std::endl;
	if (client.has_Pass && client.has_Nick && client.has_User && !client.regist)
		regist_Client(client);
}

void Server::regist_Client(Client &client)
{
	client.regist = true;
	std::string welcome = ":ircserver 001 " + client.getNickname() + " :Welcome on the server IRC\r\n";
	send(client.getFd(), welcome.c_str(), welcome.size(), 0);
	std::cout << "Client FD " << client.getFd() << " registered !" << std::endl;
}

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

