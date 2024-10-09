#include <Client.hpp>

Client::Client(){
	this->_socket = -1;
	this->_nickname = "";
	this->_username = "";
	this->_isOperator = false;
	this->_id = -1;
}

Client::Client(int socket, int id){
	this->_socket = socket;
	this->_nickname = "";
	this->_username = "";
	this->_isOperator = false;
	this->_id = id;
}

Client::Client(const Client &copy){
	*this = copy;
}

Client &Client::operator=(const Client &copy){
	if (this != &copy){
		this->_socket = copy._socket;
		this->_nickname = copy._nickname;
		this->_username = copy._username;
		this->_isOperator = copy._isOperator;
		this->_id = copy._id;
	}
	return *this;
}

Client::~Client(){
}

// Getters

int Client::getSocket(void) const{
	return this->_socket;
}

std::string Client::getNickname(void) const{
	return this->_nickname;
}

std::string Client::getUsername(void) const{
	return this->_username;
}

bool Client::getIsOperator(void) const{
	return this->_isOperator;
}

// Setters

void Client::setSocket(int socket){
	this->_socket = socket;
}

void Client::setNickname(std::string nickname){
	this->_nickname = nickname;
}

void Client::setUsername(std::string username){
	this->_username = username;
}

void Client::setIsOperator(bool isOperator){
	this->_isOperator = isOperator;
}
