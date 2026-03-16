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
