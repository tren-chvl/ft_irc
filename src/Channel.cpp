#include "../includes/irc.hpp"

Channel::Channel(const std::string &n) : name(n)
{}

const std::string &Channel::getName() const
{
	return (name);
}

const std::string &Channel::getMode() const
{
	return (mode);
}

void Channel::setMode(const std::string &M)
{
	mode = M;
}

void Channel::addMember(int fd)
{
	member.insert(fd);
	if (member.size() == 1)
		operators.insert(fd);
}

void Channel::removeMember(int fd)
{
	member.erase(fd);
	operators.erase(fd);
}

bool Channel::isMember(int fd) const
{
	return member.count(fd);
}

bool Channel::isOperator(int fd) const
{
	return operators.count(fd);
}

const std::set<int> &Channel::getMember() const
{
	return member;
}
