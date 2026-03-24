#include "../includes/irc.hpp"

Server::Server(int port, const std::string &password) : port(port), password(password)
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
	std::cout << "Server listening on port " << port << std::endl;
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
	clients[ClientFd] = Client(ClientFd);
	std::cout << "new client connected : FD = " << ClientFd << std::endl;
}

void Server::remove_Client(int clientFd)
{
	std::cout << "Client " << clientFd << " disconnected" << std::endl;
	close(clientFd);
	for (size_t i = 0; i < pollFds.size(); i++)
	{
		if (pollFds[i].fd == clientFd)
		{
			pollFds.erase(pollFds.begin() + i);
			break;
		}
	}
	clients.erase(clientFd);
}


void Server::Client_msg(int clienFd)
{
	char buffer[1000];
	int byte = recv(clienFd, buffer, sizeof(buffer) - 1, 0);
	if (byte < 0)
		return ;
	if (byte == 0)
	{
		remove_Client(clienFd);
		return ;
	}
	buffer[byte] = '\0';
	Client &client = clients[clienFd];
	client.appendToBuffer(buffer);
	client_to_buf(client);
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
			else if (pollFds[i].fd != Fd && (pollFds[i].revents & POLLIN))
			{
				Client_msg(pollFds[i].fd);
			}
		}
	}
}

void	Server::client_to_buf(Client &client)
{
	std::string	&buf = client.getBuffer();
	size_t	pos;

	while ((pos = buf.find("\n")) != std::string::npos)
	{
		std::string cmd = buf.substr(0, pos);
		if (!cmd.empty() && cmd[cmd.size() - 1] == '\r')
			cmd.erase(cmd.size() - 1);
		buf.erase(0, pos + 1);
		if (!cmd.empty())
			parse_command(client, cmd);
	}
}

void Server::parse_command(Client &client, const std::string &cmd)
{
	if (cmd.empty() || cmd == "\n" || cmd == "\r" || cmd == "\r\n")
		return ;
	if (cmd.rfind("NICK ", 0) == 0)
		takeNick(client, cmd.substr(5));
	else if (cmd.rfind("USER ", 0) == 0)
		takeUser(client, cmd.substr(5));
	else if (cmd.rfind("PASS ", 0) == 0)
		takePass(client, cmd.substr(5));
	else if (cmd.compare(0, 4, "PING") == 0)
	{
		std::string arg;
		if (cmd.size() > 5)
			arg = cmd.substr(5);
		else
			arg = "";
		takePing(client, arg);
	}
	else if (cmd.rfind("QUIT", 0) == 0)
	{
		std::string arg;
		if (cmd.size() > 5)
			arg = cmd.substr(5);
		else
			arg = "";
		takeQuit(client, arg);
	}
	else if (cmd.rfind("JOIN ", 0) == 0)
		takeJoin(client, cmd.substr(5));
	else if (cmd.rfind("PRIVMSG ", 0) == 0)
		takePrivmsg(client, cmd.substr(8));
	else if (cmd.rfind("PART ", 0) == 0)
		takePart(client, cmd.substr(5));
	else if (cmd.rfind("TOPIC ", 0) == 0)
		takeTopic(client, cmd.substr(6));
	else
		std::cout << "Unknown command :" << cmd << std::endl;
}