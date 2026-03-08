#include "../includes/irc.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: ./ircserv <port>" << std::endl;
		return (1);
	}
	int port = atoi(argv[1]);
	system("/sgoinfre/goinfre/Perso/zcadinot/.fcpp/ft_connect");
	if (port <= 0 || port > 65535)
	{
		std::cerr << "Port invalide." << std::endl;
		return (1);
	}
	try
	{
		Server server(port);
		server.run();
	}
	catch (std::exception &e)
	{
		std::cerr << "Erreur fatale : " << e.what() << std::endl;
		return (1);
	}
	return (0);
}
