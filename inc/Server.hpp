#ifndef SERVER_HPP
#define SERVER_HPP

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
#include <Channel.hpp>
#include <Authentication.hpp>

#define MAX_CLIENTS 42
#define MAX_MSG_SIZE 4096

#define SRV_NAME "Anon Chat Server"

class Server {
	private:
		int 							_port;
		int 							_socket;
		struct	sockaddr_in				_servAddr;
		std::map<int, Client>			_clients;
		std::map<std::string, Channel>	_channels;
		std::string						_password;
	public:
		Server();
		Server(int port);
		Server(const Server &copy);
		Server &operator=(const Server &copy);
		~Server();
		// Getters
		int getPort(void) const;
		int getSocket(void) const;
		std::map<int, Client> getClients(void) const;
		std::map<std::string, Channel> getChannels(void) const;
		std::string getPassword(void) const;
		// Setters
		void setPort(int port);
		void setSocket(int socket);
		void setClients(std::map<int, Client> clients);
		void setChannels(std::map<std::string, Channel> channels);
		// Methods
		void start(void);
};

#endif