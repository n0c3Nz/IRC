#include <Channel.hpp>

Channel::Channel() {
	this->_name = "Default";
	this->_isPrivate = false;
}

Channel::Channel(std::string name) {
	this->_name = name;
	this->_isPrivate = false;
}

Channel::Channel(const Channel &copy) {
	*this = copy;
}

Channel &Channel::operator=(const Channel &copy) {
	if (this != &copy) {
		this->_name = copy._name;
		this->_isPrivate = copy._isPrivate;
	}
	return *this;
}

Channel::~Channel() {
}

std::string		Channel::getName() const
{
	return this->_name;
}

bool	Channel::alreadyIn(const std::string &nickName)
{
	for (size_t i = 0; i <= this->_members.size(); i++)
		if (this->_members[i].getNickname() == nickName)
			return true;
	return false;
}
void		Channel::addClient(Client &client)
{
 	if (!alreadyIn(client.getNickname()))
 		_members.push_back(client);
 	std::cerr << "[DEBUG] Cliente añadido: " << client.getNickname() << " al canal: " << this->_name << std::endl;
 }

int		Channel::findUserFd(const std::string &nick) const
{
	for (size_t i = 0; i <= this->_members.size(); i++)
		if (this->_members[i].getNickname() == nick)
			return this->_members[i].getSocket();
	return 0;
}

std::string	Channel::getUserByNick(const std::string &nickName) const
{
	for (size_t i = 0; i <= this->_members.size(); i++)
		if (this->_members[i].getNickname() == nickName)
			return this->_members[i].getUsername();
	return 0;
}

void		Channel::sendMsg(const std::string &senderNick, const std::string &msg)
{
	int	senderFd = findUserFd(senderNick);
	std::string senderUser = getUserByNick(senderNick);
	std::string fullMsg = ":" + senderNick + "!" + senderUser + "@127.0.0.1" + " PRIVMSG #" + this->getName() + " :" + msg + "\r\n";
	for (size_t i = 0; i <= this->_members.size(); i++)
	{
		int receiverFd = findUserFd(this->_members[i].getNickname());
		if (receiverFd != senderFd && receiverFd != -1)
		{
			send(receiverFd, fullMsg.c_str(), fullMsg.size(), 0);
		}
	}
}