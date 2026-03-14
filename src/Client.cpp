#include "../includes/irc.hpp"


Client::Client(int fd) : fd(fd), nickname(""), username(""), realname(""), buffer(""), has_Pass(false), has_Nick(false), has_User(false), regist(false)
{}

int Client::getFd() const
{
	return (fd);
}

std::string Client::getNickname() const
{
	return (nickname);
}

std::string Client::getUsername() const
{
	return (username);
}

std::string Client::getRealname() const
{
	return (realname);
}

std::string Client::getBuffer() const
{
	return (buffer);
}

std::string &Client::getBufferr()
{
	return (buffer);
}

void Client::setNickname(const std::string& nick)
{
	nickname = nick;
}

void Client::setUsername(const std::string& user)
{
	username = (user);
}

void Client::setRealname(const std::string& real)
{
	realname = (real);
}

void Client::appendToBuffer(const std::string& data)
{
	buffer += data;
}

void Client::clearBuffer()
{
	buffer.clear();
}
