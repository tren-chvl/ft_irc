#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client
{
	private:
		int	fd;
		std::string nickname;
		std::string username;
		std::string realname;
		std::string buffer;

	public:
		Client(int fd);

	int			getFd() const;
	std::string	getNickname() const;
	std::string	getUsername() const;
	std::string	getRealname() const;
	std::string	getBuffer() const;
	std::string &getBufferr();
	void		setNickname(const std::string& nick);
	void		setUsername(const std::string& user);
	void		setRealname(const std::string& real);
	void		appendToBuffer(const std::string& data);
	void		clearBuffer();
	bool		has_Pass;
	bool		has_Nick;
	bool		has_User;
	bool		regist;
};

#endif
