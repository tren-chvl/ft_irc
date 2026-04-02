#include "../../includes/irc.hpp"

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
