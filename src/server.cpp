/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zcadinot <zcadinot@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 15:35:33 by zcadinot          #+#    #+#             */
/*   Updated: 2026/03/28 15:35:35 by zcadinot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/irc.hpp"
#include <errno.h>


extern bool sign;

Server::Server(int port, const std::string &password) : port(port), Fd(-1) ,password(password), serverName("ircserv")
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
	{
		close(Fd);
		throw std::runtime_error("fcntl() failed");
	}
	setsockopt(Fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	if (bind(Fd, (sockaddr*)&addr, sizeof(addr)) < 0)
	{
		close(Fd);
		throw std::runtime_error("bind() failed");
	}
	if (listen(Fd, 10) < 0)
	{
		close(Fd);
		throw std::runtime_error("listen() failed");
	}
	pollfd pfd;
	pfd.fd = Fd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	pollFds.push_back(pfd);
	std::cout << "Server listening on port " << port << std::endl;
}

void Server::acceptClient()
{
	int ClientFd = accept(Fd, NULL, NULL);
	if (ClientFd < 0)
	{
		if (errno == EMFILE)
			std::cerr << "Too many open files: cannot accept more clients" << std::endl;
		return;
	}
	fcntl(ClientFd, F_SETFL, O_NONBLOCK);
	pollfd poll_fd;
	poll_fd.fd = ClientFd;
	poll_fd.events = POLLIN;
	poll_fd.revents = 0;
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
			break ;
		}
	}
	clients.erase(clientFd);
}


void Server::regist_Client(Client &client)
{
	client.regist = true;
	std::string welcome = ":ircserver 001 " + client.getNickname() + " :Welcome on the server IRC\r\n";
	send(client.getFd(), welcome.c_str(), welcome.size(), 0);
	std::cout << "Client FD " << client.getFd() << " registered !" << std::endl;
}

void Server::Client_msg(int clientFd)
{
	char buffer[1000];
	bool removed;
	int byte = recv(clientFd, buffer, sizeof(buffer) - 1, 0);

	if (byte <= 0)
	{
		remove_Client(clientFd);
		return;
	}
	buffer[byte] = '\0';
	std::map<int, Client>::iterator it = clients.find(clientFd);
	if (it == clients.end())
		return;
	it->second.appendToBuffer(buffer);
	removed = client_to_buf_safe(clientFd);
	if (removed)
		return;
	it = clients.find(clientFd);
	if (it != clients.end() && it->second.Do_Disco())
		remove_Client(clientFd);
}

bool Server::client_to_buf_safe(int clientFd)
{
	size_t pos;
	size_t len;
	std::string cmd;

	while (true)
	{
		std::map<int, Client>::iterator it = clients.find(clientFd);
		if (it == clients.end())
			return (true);
		Client &client = it->second;
		std::string &buf = client.getBuffer();
		pos = buf.find("\r\n");
		len = 2;
		if (pos == std::string::npos)
		{
			pos = buf.find("\n");
			len = 1;
		}
		if (pos == std::string::npos)
			break ;
		cmd = buf.substr(0, pos);
		buf.erase(0, pos + len);
		if (!cmd.empty() && cmd[cmd.size() - 1] == '\r')
			cmd.erase(cmd.size() - 1);
		if (!cmd.empty())
		{
			parse_command(client, cmd);
			if (clients.find(clientFd) == clients.end())
				return (true);
		}
	}
	return (false);
}

void Server::run() 
{
	int fd;
	int ret;

	while (sign)
	{
		ret = poll(&pollFds[0], pollFds.size(), 100);
		if (!sign)
			break ;
		if (ret < 0)
			continue ;
		for (size_t i = 0; i < pollFds.size(); i++)
		{
			fd = pollFds[i].fd;
			if (fd == Fd && (pollFds[i].revents & POLLIN))
				acceptClient();
			else if (fd != Fd && (pollFds[i].revents & POLLIN))
			{
				Client_msg(fd);
				if (clients.find(fd) == clients.end())
				{
					i--;
					continue ;
				}
			}
		}
	}
	std::cout << "Server shutting down !" << std::endl;
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
		close(it->first);
	clients.clear();
	pollFds.clear();
}

void Server::client_to_buf(Client &client)
{
	std::string	cmd;
	std::string	&buf = client.getBuffer();
	size_t		pos;

	while (true)
	{
		pos = buf.find("\r\n");
		size_t len = 2;
		if (pos == std::string::npos)
		{
			pos = buf.find("\n");
			len = 1;
		}
		if (pos == std::string::npos)
			break ;
		cmd = buf.substr(0, pos);
		buf.erase(0, pos + len);
		if (!cmd.empty() && cmd[cmd.size() - 1] == '\r')
			cmd.erase(cmd.size() - 1);
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
	else if (cmd.rfind("KICK ", 0) == 0)
		takeKick(client, cmd.substr(5));
	else if (cmd.rfind("MODE ", 0) == 0)
		takeMode(client, cmd.substr(5));
	else if (cmd.rfind("INVITE ", 0) == 0)
		takeInvite(client, cmd.substr(7));
	else
		std::cout << "Unknown command :" << cmd << std::endl;
}
