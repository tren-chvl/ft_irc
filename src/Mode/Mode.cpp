#include "../../includes/irc.hpp"

int Server::getFdByNick(const std::string &nick)
{
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second.getNickname() == nick)
			return (it->first);
	}
	return (-1);
}

bool Server::parseModeArguments(const std::string &arg, std::string &chanName, std::string &modes, std::vector<std::string> &params)
{
	std::stringstream ss(arg);
	ss >> chanName;
	ss >> modes;
	if (chanName.empty() || modes.empty())
		return false;
	std::string p;
	while (ss >> p)
		params.push_back(p);
	return (true);
}

void Server::applySingleMode(Channel &chan, char sign, char mode, const std::string &param)
{
	if (sign == '+')
	{
		if (mode == 'i')
			chan.addMode('i');
		else if (mode == 't')
			chan.addMode('t');
		else if (mode == 'k')
		{
			chan.addMode('k');
			chan.setKey(param);
		}
		else if (mode == 'l')
		{
			chan.addMode('l');
			chan.setLimit(atoi(param.c_str()));
		}
		else if (mode == 'o')
		{
			int fd = getFdByNick(param);
			if (fd > 0 && chan.isMember(fd))
				chan.addOperator(fd);
		}
	}
	else if (sign == '-')
	{
		if (mode == 'i')
			chan.removeMode('i');
		else if (mode == 't')
			chan.removeMode('t');
		else if (mode == 'k')
		{
			chan.removeMode('k');
			chan.setKey("");
		}
		else if (mode == 'l')
		{
			chan.removeMode('l');
			chan.setLimit(-1);
		}
		else if (mode == 'o')
		{
			int fd = getFdByNick(param);
			if (fd > 0)
				chan.removeOperator(fd);
		}
	}
}

void Server::applyAllModes(Channel &chan, const std::string &modes, const std::vector<std::string> &params)
{
	char sign = 0;
	size_t paramIndex = 0;
	char c;

	for (size_t i = 0; i < modes.size(); i++)
	{
		c = modes[i];
		if (c == '+' || c == '-')
		{
			sign = c;
			continue ;
		}
		std::string param = "";
		if (c == 'k' || c == 'l' || c == 'o')
		{
			if (paramIndex < params.size())
				param = params[paramIndex++];
		}
		applySingleMode(chan, sign, c, param);
	}
}

void Server::broadcastModeChange(Channel &chan, const std::string &msg)
{
	for (std::set<int>::const_iterator it = chan.getMember().begin(); it != chan.getMember().end(); ++it)
		send(*it, msg.c_str(), msg.size(), 0);
}

void Server::takeMode(Client &client, const std::string &arg)
{
	if (!client.regist)
		return ;
	std::string chanName;
	std::string error;
	std::string modes;
	std::string msg;
	std::vector<std::string> params;
	if (!parseModeArguments(arg, chanName, modes, params))
		return;
	if (!channels.count(chanName))
		return;
	Channel &chan = channels.find(chanName)->second;
	if (!chan.isOperator(client.getFd()))
	{
		error = ":server 482 " + client.getNickname() + " " + chanName + " :You're not channel operator\r\n";
		send(client.getFd(), error.c_str(), error.size(), 0);
		return ;
	}
	applyAllModes(chan, modes, params);
	msg = ":" + client.getNickname() + " MODE " + chanName + " " + arg.substr(chanName.size() + 1) + "\r\n";
	broadcastModeChange(chan, msg);
}
