#include <Client.hpp>

Client::Client(){
	this->_socket = -1;
	this->_nickname = "";
	this->_username = "";
	this->_isOperator = false;
	this->_buffer = "";
	this->_isAuth = false;
	this->_pwdSent = false;
}

Client::Client(int socket){
	this->_socket = socket;
	this->_nickname = "Guest" + std::to_string(socket);
	this->_username = "";
	this->_isOperator = false;
	this->_buffer = "";
	this->_isAuth = false;
	this->_pwdSent = false;
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

std::string Client::getRealname(void) const{
	return this->_realname;
}

bool Client::getIsOperator(void) const{
	return this->_isOperator;
}

bool Client::getIsAuth(void) const{
	return this->_isAuth;
}

bool Client::getPwdSent(void) const{
	return this->_pwdSent;
}

std::string Client::getHash(void) const{
	return this->_nickname + ":" + this->_username + ":" + this->_realname;
}

std::vector<std::string> Client::getJoinedChannels(void) const{
	return this->_joinedChannels;
}

std::string Client::getHost(void) const {
	struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    if (getpeername(_socket, (struct sockaddr *)&clientAddr, &clientAddrLen) == -1)
		throw ErrorHandler::GetPeerName();
	return inet_ntoa(clientAddr.sin_addr);
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

void Client::setRealname(std::string realname){
	this->_realname = realname;
}

void Client::setIsAuth(void){
	if (_username.empty() || _realname.empty()){
		this->_isAuth = false;
		return;
	}
	this->_isAuth = true;
}

void Client::setPwdSent(void){
	this->_pwdSent = true;
}

bool Client::alreadyJoined(const std::string &channelName) const
{
	for (size_t i; i < _joinedChannels.size(); i++)
	{
		if (std::strcmp(_joinedChannels[i].c_str(), channelName.c_str()) == 0)
			return true;
	}
	return false;
}

void	Client::joinChannel(Channel &channel)
{
	if (alreadyJoined(channel.getName()) == false)
		_joinedChannels.push_back(channel.getName());
}

void Client::leaveChannel(const std::string &channelName)
{
	for (size_t i = 0; i < _joinedChannels.size(); i++)
	{
		if (std::strcmp(_joinedChannels[i].c_str(), channelName.c_str()) == 0)
		{
			_joinedChannels.erase(_joinedChannels.begin() + i);
			return;
		}
	}
}