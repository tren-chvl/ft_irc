#include "../../includes/irc.hpp"


// void Server::takePass(Client &client, const std::string &arg)
// {
// 	if (client.has_Pass)
// 		return;
// 	if (arg.empty())
// 	{
// 		std::cout << "PASS missing for FD " << client.getFd() << std::endl;
// 		remove_Client(client.getFd());
// 		return;
// 	}
// 	if (arg != password)
// 	{
// 		std::cout << "Wrong PASS for FD " << client.getFd() << std::endl;
// 		remove_Client(client.getFd());
// 		return;
// 	}
// 	client.has_Pass = true;
// 	std::cout << "PASS OK for FD " << client.getFd() << std::endl;
// }

void Server::takePass(Client &client, const std::string &arg)
{
	if (client.regist)
		return (sendError(client, "462", ":You may not reregister"));

	std::string pass = arg;
	while (!pass.empty() && (pass[0] == ' ' || pass[0] == '\t'))
		pass.erase(0, 1);
	while (!pass.empty() && (pass[pass.size() - 1] == ' ' || pass[pass.size() - 1] == '\t'))
		pass.erase(pass.size() - 1);
	if (pass.size() >= 2 && pass[0] == '"' && pass[pass.size() - 1] == '"')
		pass = pass.substr(1, pass.size() - 2);
	if (pass.empty())
	{
		sendError(client, "461", "PASS :Not enough parameters");
		client.Disco_down();
		return ;
	}
	if (pass != password)
	{
		std::cout << "Wrong PASS for FD " << client.getFd() << std::endl;
		client.Disco_down();
		return ;
	}
	std::cout << "PASS OK for FD " << client.getFd() << std::endl;
	client.has_Pass = true;
}
