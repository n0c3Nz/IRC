#include <Server.hpp>

Server::Server(){
	this->_port = 4242;
	this->_socket = -1;
	_servAddr.sin_family = AF_INET;//IPv4
	_servAddr.sin_addr.s_addr = INADDR_ANY;// Para escuchar en todas las interfaces
	_servAddr.sin_port = htons(this->_port);
}

Server::Server(int port){
	this->_port = port;
	this->_socket = -1;
	_servAddr.sin_family = AF_INET;//IPv4
	_servAddr.sin_addr.s_addr = INADDR_ANY;// Para escuchar en todas las interfaces
	_servAddr.sin_port = htons(this->_port);
}

Server::Server(const Server &copy){
	*this = copy;
}

Server &Server::operator=(const Server &copy){
	if (this != &copy){
		this->_port = copy._port;
		this->_socket = copy._socket;
		this->_clients = copy._clients;
		this->_channels = copy._channels;
	}
	return *this;
}

Server::~Server(){
}

// Getters

int Server::getPort(void) const{
	return this->_port;
}

int Server::getSocket(void) const{
	return this->_socket;
}

std::map<int, Client> Server::getClients(void) const{
	return this->_clients;
}

std::map<std::string, Channel> Server::getChannels(void) const{
	return this->_channels;
}

std::string Server::getPassword(void) const{
	return this->_password;
}

// Setters

void Server::setPort(int port){
	this->_port = port;
}

void Server::setSocket(int socket){
	this->_socket = socket;
}

void Server::setClients(std::map<int, Client> clients){
	this->_clients = clients;
}

void Server::setChannels(std::map<std::string, Channel> channels){
	this->_channels = channels;
}

// Methods

void Server::start(void) {
    std::cout << "Server started" << std::endl;
    this->_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_socket == -1) {
        throw ErrorHandler::SocketCreation();
    }

    if (bind(this->_socket, (struct sockaddr *)&_servAddr, sizeof(_servAddr)) == -1) {
        throw ErrorHandler::SocketBind();
    }

    if (listen(this->_socket, MAX_CLIENTS) == -1) {
        throw ErrorHandler::SocketListen();
    }

    std::cout << "Server listening on port " << this->_port << std::endl;
}
