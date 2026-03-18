#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <set>
#include <string>

class Client;

class Channel
{
	private:
		std::string name;
		std::set<int> member;
		std::set<int> operators;
		std::string mode;
	public:
		Channel(const std::string &name);
		const std::string &getName() const;
		const std::string &getMode() const;
		void setMode(const std::string &M);
		void addMember(int fd);
		void removeMember(int fd);
		bool isMember(int fd) const;
		bool isOperator(int fd) const;
		const std::set<int> &getMember() const;
};

#endif