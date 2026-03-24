#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <set>
#include <string>

class Client;

class Channel
{
private:
	std::string		name;
	std::set<int>	member;
	std::set<int>	operators;
	std::set<int>	invited;
	std::string		mode;
	std::string		topic;
	std::string		key;
	int				limit; 

public:
	Channel(const std::string &name);
	const std::string &getName() const;
	const std::set<int> &getMember() const;

	void	addMember(int fd);
	void	removeMember(int fd);
	bool	isMember(int fd) const;

	bool	isOperator(int fd) const;
	void	addOperator(int fd);
	void	removeOperator(int fd);

	void addInvited(int fd);
	bool isInvited(int fd) const;
	void clearInvited();

	const std::string &getTopic() const
	{
		return (topic);
	}
	void setTopic(const std::string &t)
	{
		topic = t;
	}
	const std::string &getMode() const;
	void setMode(const std::string &m);
	void addMode(char c);
	void removeMode(char c);
	bool hasMode(char c) const;

	void setKey(const std::string &k);
	const std::string &getKey() const;

	void setLimit(int l);
	int getLimit() const;
};

#endif