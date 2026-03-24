#include "../includes/irc.hpp"

Channel::Channel(const std::string &name): name(name), member(), operators(), invited(), mode(""), topic(""), key(""), limit(-1)
{}

const	std::string &Channel::getName() const
{
	return (name);
}

const	std::set<int> &Channel::getMember() const
{
	return (member);
}

void	Channel::addMember(int fd)
{
	member.insert(fd);
	if (member.size() == 1)
		operators.insert(fd);
}

void	Channel::removeMember(int fd)
{
	member.erase(fd);
	operators.erase(fd);
	invited.erase(fd);
}

bool	Channel::isMember(int fd) const
{
	return member.count(fd);
}

bool	Channel::isOperator(int fd) const
{
	return operators.count(fd);
}

void	Channel::addOperator(int fd)
{
	operators.insert(fd);
}

void	Channel::removeOperator(int fd)
{
	operators.erase(fd);
}

void	Channel::addInvited(int fd)
{
	invited.insert(fd);
}

bool	Channel::isInvited(int fd) const
{
	return invited.count(fd);
}

void	Channel::clearInvited()
{
	invited.clear();
}

const	std::string &Channel::getMode() const
{
	return (mode);
}

void	Channel::setMode(const std::string &m)
{
	mode = m;
}

void	Channel::addMode(char c)
{
	if (mode.find(c) == std::string::npos)
		mode += c;
}

void	Channel::removeMode(char c)
{
	size_t pos = mode.find(c);
	if (pos != std::string::npos)
		mode.erase(pos, 1);
}

bool	Channel::hasMode(char c) const
{
	return mode.find(c) != std::string::npos;
}

void	Channel::setKey(const std::string &k)
{
	key = k;
}

const	std::string &Channel::getKey() const
{
	return (key);
}

void	Channel::setLimit(int l)
{
	limit = l;
}

int	Channel::getLimit() const
{
	return (limit);
}

