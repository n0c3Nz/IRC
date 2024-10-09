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