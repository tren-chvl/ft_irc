#include "../../includes/irc.hpp"

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
