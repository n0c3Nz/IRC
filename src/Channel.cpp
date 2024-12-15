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

static	bool	alreadyIn(std::vector<std::string> members, std::string pending)
{
	for (size_t i = 0; i < members.size(); i++)
		if (members[i] == pending)
			return true;
	return false;
}

void		Channel::addClient(Client &client)
{
	if (!alreadyIn(_members, client.getNickname()))
		_members.push_back(client.getNickname());
	std::cerr << "[DEBUG] Cliente añadido: " << client.getNickname() << " al canal: " << this->_name << std::endl;
}