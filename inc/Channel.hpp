#ifndef CHANNEL_HPP
#define CHANNEL_HPP

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
#include "Client.hpp"

class Client;

class Channel {
	private:
		std::string						_name;
		std::string						_topic;
		std::vector<Client>				_members;
		std::vector<std::string>		_operators;
		std::string						_password;
		bool							_isPrivate;
		bool							_Pwd;
		char							_mode;
	public:
		Channel();
		Channel(std::string name);
		Channel(const Channel &copy);
		Channel &operator=(const Channel &copy);
		~Channel();
		std::string		getName() const;
		std::vector<Client>	getMembers() const;
		bool			getIfPwd() const;
		std::string		getPwd() const;
		void			addClient(Client &client);
		void			removeClient(Client &client);
		void			sendMsg(const std::string &senderNick, const std::string &host,const std::string &msg);
		int 			findUserFd(const std::string &senderNick) const;
		std::string		getUsernameByNick(const std::string &nickName) const;
		bool			alreadyIn(const std::string &nickName);
};


#endif