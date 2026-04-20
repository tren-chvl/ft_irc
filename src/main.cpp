#include "../includes/irc.hpp"
#include <iostream>
#include <cstdlib>

bool sign = true;

void handle_sigint(int)
{
	sign = false;
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return (1);
	}
	int port = atoi(argv[1]);
	if (port <= 0 || port > 65535)
	{
		std::cerr << "Invalid port." << std::endl;
		return (1);
	}
	std::string password = argv[2];
	while(!password.empty() && isspace(password[0]))
		password.erase(0, 1);
	while(!password.empty() && isspace(password[password.size() - 1]))
		password.erase(password.size() - 1);
	if (password.empty())
	{
		std::cerr << "Error: password cannot be empty or space only." << std::endl;
		return (1);
	}
	try
	{
		signal(SIGINT, handle_sigint);
		Server server(port, password);
		server.run();
	}
	catch (std::exception &e)
	{
		std::cerr << "Fatal error : " << e.what() << std::endl;
		return (1);
	}
	return (0);
}