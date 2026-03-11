#include "../includes/irc.hpp"

Server::Server(int port) : port(port) 
{
	initSocket();
}

Server::~Server() 
{
	close(Fd);
}

void Server::initSocket()
{
	int opt = 1;
	Fd = socket(AF_INET, SOCK_STREAM, 0);
	if (Fd < 0)
		throw std::runtime_error("socket() failed");
	if (fcntl(Fd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("fcntl() failed");
	setsockopt(Fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	if (bind(Fd, (sockaddr*)&addr, sizeof(addr)) < 0)
		throw std::runtime_error("bind() failed");
	if (listen(Fd, 10) < 0)
		throw std::runtime_error("listen() failed");
	pollfd pfd;
	pfd.fd = Fd;
	pfd.events = POLLIN;
	pollFds.push_back(pfd);
	std::cout << "Serveur en ecoute sur le port " << port << std::endl;
}

void Server::acceptClient()
{
	int ClientFd = accept(Fd, NULL, NULL);
	if (ClientFd < 0)
		return ;
	fcntl(ClientFd, F_SETFL, O_NONBLOCK);
	pollfd poll_fd;
	poll_fd.fd = ClientFd;
	poll_fd.events = POLLIN;
	pollFds.push_back(poll_fd);
	std::cout << "new client connected : FD = " << ClientFd << std::endl;
}

void Server::run() 
{
	while (true)
	{
		int ret = poll(&pollFds[0], pollFds.size(), -1);
		if (ret < 0)
			continue ;
		for (size_t i = 0;i < pollFds.size(); i++)
		{
			if (pollFds[i].fd == Fd && (pollFds[i].revents & POLLIN))
			{
				acceptClient();
			}
		}
	}
}

