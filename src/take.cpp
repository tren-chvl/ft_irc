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
	{
		std::cout << "PASS already sent by FD " << client.getFd() << std::endl;
		return;
	}
	if (arg.empty())
	{
		std::cout << "PASS empty for FD " << client.getFd() << std::endl;
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
	{
		std::cout << "NICK refused (PASS missing) for FD " << client.getFd() << std::endl;
		return;
	}
	if (arg.empty())
	{
		std::cout << "NICK empty for FD " << client.getFd() << std::endl;
		return;
	}
	client.setNickname(arg);
	client.has_Nick = true;
	std::cout << "NICK set: " << arg << " for FD " << client.getFd() << std::endl;
	if (client.has_Pass && client.has_Nick && client.has_User && !client.regist)
		regist_Client(client);
}

void Server::takeUser(Client &client, const std::string &arg)
{
	if (!client.has_Pass)
	{
		std::cout << "USER refused (PASS missing) for FD " << client.getFd() << std::endl;
		return;
	}
	if (client.has_User)
	{
		std::cout << "USER already set for FD " << client.getFd() << std::endl;
		return;
	}
	if (arg.empty())
	{
		std::cout << "USER empty for FD " << client.getFd() << std::endl;
		return;
	}
	std::stringstream ss(arg);
	std::string username;
	ss >> username;
	size_t pos = arg.find(":");
	std::string realname = (pos != std::string::npos) ? arg.substr(pos + 1) : "";
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
		return;
	if (arg.empty())
		return;
	std::string chanName = arg;
	if (chanName[0] != '#')
		chanName = "#" + chanName;
	if (!channels.count(chanName))
		channels.insert(std::make_pair(chanName, Channel(chanName)));
	Channel &chan = channels.find(chanName)->second;
	if (chan.isMember(client.getFd()))
		return;
	chan.addMember(client.getFd());
	std::string joinmsg = ":" + client.getNickname() + " JOIN " + chanName + "\r\n";
	for (std::set<int>::const_iterator it = chan.getMember().begin();it != chan.getMember().end(); ++it)
		send(*it, joinmsg.c_str(), joinmsg.size(), 0);
	std::string names = "= " + chanName + " :";
	for (std::set<int>::const_iterator it = chan.getMember().begin();it != chan.getMember().end(); ++it)
		names += clients[*it].getNickname() + " ";
	names += "\r\n";
	send(client.getFd(), names.c_str(), names.size(), 0);
	std::cout << "FD " << client.getFd() << " joined " << chanName << std::endl;
}

void	Server::takeTopic(Client &client, const std::string &arg)
{
	if (!client.regist)
		return ;
	size_t space = arg.find(' ');
	std::string chanName = arg.substr(0, space);
	if (!channels.count(chanName))
		return ;
	Channel &chan = channels.find(chanName)->second;
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
	std::string newTopic = arg.substr(space + 1);
	if (!newTopic.empty() && newTopic[0] == ':')
		newTopic.erase(0, 1);
	chan.setTopic(newTopic);
	std::string msg = ":" + client.getNickname() + " TOPIC " + chanName + " :" + newTopic + "\r\n";
	for (std::set<int>::const_iterator it = chan.getMember().begin();it != chan.getMember().end(); ++it)
		send(*it, msg.c_str(), msg.size(), 0);
}

void Server::takeKick(Client &client, const std::string &arg)
{
	//verifier si ya probleme si caillou pas bien caller !!!

	if (!client.regist || arg.empty())
	{
		return ;
	}
	std::string channelName;
	std::string name;
	std::string reason;

	//en gros la on vas set les arg a des variable

	size_t arg1 = arg.find(" ", 0);
	channelName = arg.substr(0, arg1);

	size_t arg2 = arg.find(" ", arg1 + 1);
	name = arg.substr(arg1 + 1, arg2 - (arg1 + 1));

	//verifier si reason mit 
	
	if (arg2 == std::string::npos)
	{
		name = arg.substr(arg1 + 1);
		reason = "";
	}
	else
	{
		reason = arg.substr(arg2 + 1);
	}

	// ici on netoyer reason pour enlver : si possible

	if (!reason.empty() && reason[0] == ':')
		reason.erase(0, 1);

	if (!channels.count(channelName))
	{
		// channe nom invalide
		return;
	}

	// Recuper est initialise le channel
	std::map<std::string, Channel>::iterator it = channels.find(channelName);
	if (it == channels.end())
		return;
	Channel &chan = it->second;

	// check operator est membre
	if (!chan.isMember(client.getFd()))
		return;
	if (!chan.isOperator(client.getFd()))
		return;

	// Trouver le target
	int targetFd = -1;

	for (std::map<int, Client>::iterator itc = clients.begin(); itc != clients.end(); ++itc)
	{
		if (itc->second.getNickname() == name)
		{
			targetFd = itc->first;
			break;
		}
	}

	if (targetFd == -1)
		return;

	// verifier quil est dans le channel
	if (!chan.isMember(targetFd))
		return;

	// envoyer le message de kick
	std::string msg = ":" + client.getNickname() + " KICK " + channelName + " " + name;

	if (!reason.empty())
		msg += " :" + reason;

	msg += "\r\n";

	for (std::set<int>::const_iterator it2 = chan.getMember().begin();
	it2 != chan.getMember().end(); ++it2)
	{
		send(*it2, msg.c_str(), msg.size(), 0);
	}

	// cho bail le mec mdrr
	chan.removeMember(targetFd);

	// si channel vide on le supprime
	if (chan.getMember().empty())
		channels.erase(channelName);

	return ;
}
