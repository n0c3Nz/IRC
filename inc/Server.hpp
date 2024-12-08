#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <vector>
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
#include <poll.h>
#include <sys/epoll.h>
#include <memory>

#include <ErrorHandler.hpp>
#include <Client.hpp>
#include <Channel.hpp>

#define MAX_EVENTS 10
#define MAX_MSG_SIZE 4096

#define SRV_NAME "Anon Chat Server"
#define SRV_VERSION "0.1"
#define RPL_WELCOME "001"
#define RPL_YOURHOST "002"
#define RPL_CREATED "003"
#define RPL_MYINFO "004"

class Server {
	private:
		int 							_port;
		struct	sockaddr_in				_servAddr;
		int 							_socket;
		int 							_epollFd;
		std::map<int, std::shared_ptr<Client>> _clients;
		std::vector <std::string>		_authenticatedClients;
		std::map<std::string, Channel>	_channels;
		std::string						_password;
		std::string						_motd;
	public:
		Server();
		Server(int port, std::string password, std::string motd);
		Server(const Server &copy);
		Server &operator=(const Server &copy);
		~Server();
		// Getters
		int getPort(void) const;
		int getSocket(void) const;
		const std::map<int, std::shared_ptr<Client>>& getClients() const;
		std::map<std::string, Channel> getChannels(void) const;
		std::string getPassword(void) const;
		// Setters
		void setPort(int port);
		void setSocket(int socket);
		void setClients(std::map<int, std::shared_ptr<Client>> clients);
		void setChannels(std::map<std::string, Channel> channels);
		void setNickname(int clientFd, const std::string &nickname);
		// Methods
		void AnnounceConnection(int clientFd) const;
		void start(void);
		void run(void);
		void handleClientData(int clientFd);
		void processCommand(int clientFd, std::string command);
		//Handshake
		void handshake(int clientFd);
		//Commands
		void quit(int clientFd);
		void nick(int clientFd, std::string nickname);
		void user(int clientFd, std::string username, std::string realname);
};

void setNonBlocking(int socketFd);
int checkEmptyAndAlnum(std::string str);

#endif