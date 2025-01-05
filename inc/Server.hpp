#ifndef SERVER_HPP
#define SERVER_HPP

#include "Includes.hpp"


class Client;
class Channel;
class Server {
	private:
		int 							_port;
		struct	sockaddr_in				_servAddr;
		int 							_socket;
		int 							_epollFd;
		std::map<int, std::shared_ptr<Client>> _clients;
		std::vector <std::string>		_authenticatedClients;
		std::vector <Channel>			_channels;
		std::string						_password;
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
		const std::vector <Channel>& getChannels() const;
		std::string getPassword(void) const;
		//esto es nuevo de la rama de c3nz :D
		std::string	getChannelMode(const std::string &channelName) const;
		std::string getChannelTopic(const std::string &channelName);
		int isChannelOperator(const std::string &channelName, std::string &nick);
		int getChannelMaxMembers(const std::string &channelName);
		std::string getChannelCreatedTime(const std::string &channelName);
		// Setters
		void setPort(int port);
		void setSocket(int socket);
		void setClients(std::map<int, std::shared_ptr<Client>> clients);
		void setChannels(std::vector <Channel> channels);
		void setNickname(int clientFd, const std::string &nickname);
		void setChannelTopic(std::string &channelName, std::string &topic);
		void setChannelMaxMembers(std::string &channelName, int maxMembers);
		// Methods
		void AnnounceConnection(int clientFd) const;
		void start(void);
		void run(void);
		void handleClientData(int clientFd);
		void processCommand(int clientFd, std::string command);
		int	checkHash(int clientFd);
		void checkOnline(int clientFd);
		void closeConnection(int clientFd);
		//Handshake
		void handshake(int clientFd);
		//Commands
		void quit(int clientFd, std::string msg);
		void nick(int clientFd, std::string nickname);
		void user(int clientFd, std::string username, std::string realname);
		void motd(int clientFd);
		void joinChannelServerSide(std::map<std::string, std::string> channelKey, int clientFd);
		void names(int clientFd, std::string channelName);
		void mode(int clientFd, std::string &channelName, std::string &modes);
		void part(int clientFd, std::string channelName, std::string msg);
		void partAll(int clientFd, std::string msg);
		//Auxiliar
		void notifyAllMembers(int clientFd, std::string &channelName, std::string &msg);
		int checkChannelExistence(int clientFd, const std::string &channelName);
		int checkChannelMembership(int clientFd, const std::string &channelName);
		void sendPrivateMessage(std::string senderNick, std::string msg, std::string receiverNick);
		void sendChannelMessage(int clientFd, const std::string &msg, const std::string &channelName);
		int  findUserByNick(const std::string &nick);
		//Channel& findOrCreateChannel(std::string channelName, int clientFd);
		void sendConfirmJoin(int clientFd, const std::string &channelName);
		std::map<std::string, std::string>		parseJoinRequets(std::string request) const;
		int		exist(const std::string &channelName) const;
		int		authenticateChannel(const Channel &channel, const std::string &password) const;
};

void setNonBlocking(int socketFd);
int checkEmptyAndAlnum(std::string str);
void deleteCarriageReturn(std::string &str);

#endif