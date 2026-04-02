NAME = ircserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
INCLUDES = -Iincludes
SRC =	src/main.cpp \
		src/server.cpp \
		src/Client.cpp \
		src/Channel.cpp \
		src/command/invite.cpp \
		src/command/join.cpp \
		src/command/kick.cpp \
		src/command/Mode.cpp\
		src/command/nick.cpp \
		src/command/part.cpp \
		src/command/pass.cpp \
		src/command/ping.cpp \
		src/command/Privmsg.cpp \
		src/command/quit.cpp \
		src/command/topic.cpp \
		src/command/user.cpp \
		src/error/error.cpp
OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
