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
#include <Client.hpp>

class Channel {
	private:
		std::string						_name;
		std::map<int, Client>			_clients;
		std::vector<int>				_operators;
		std::string						_password;
		bool							_isPrivate;
	public:
		Channel();
		Channel(std::string name);
		Channel(const Channel &copy);
		Channel &operator=(const Channel &copy);
		~Channel();
};


#endif