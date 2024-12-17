#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <cstring>
#include <ErrorHandler.hpp>
#include "Channel.hpp"

class Channel;
class Client {
	private:
		int 							_socket;
		std::string						_nickname;
		std::string						_username;
		std::string						_realname;
		bool							_isOperator;
		bool							_isAuth;
		bool							_pwdSent;
		std::vector<std::string>		_joinedChannels;
		std::string 					_activeChannel;
	public:
		std::string						_buffer;
		Client();
		Client(int socket);
		Client(const Client &copy);
		Client &operator=(const Client &copy);
		~Client();
		// Getters
		int getSocket(void) const;
		std::string getNickname(void) const;
		std::string getUsername(void) const;
		std::string getRealname(void) const;
		bool getIsOperator(void) const;
		bool getIsAuth(void) const;
		bool getPwdSent(void) const;
		std::string getHost(void) const;
		std::vector<std::string> getJoinedChannels(void) const;
		std::string getHash(void) const;
		// Setters
		void setSocket(int socket);
		void setNickname(std::string nickname);
		void setUsername(std::string username);
		void setRealname(std::string realname);
		void setIsOperator(bool isOperator);
		void setIsAuth(void);
		void setPwdSent(void);
		void	joinChannel(Channel &channel);
		void	leaveChannel(const std::string &channelName);
		bool	alreadyJoined(const std::string &channelName) const;
};


#endif
