#include <Authentication.hpp>

Authentication::Authentication() {
	std::cout << "Authentication default constructor called" << std::endl;
}

Authentication::Authentication(const Authentication &copy) {
	std::cout << "Authentication copy constructor called" << std::endl;
	if (this != &copy)
		*this = copy;
}

Authentication &Authentication::operator=(const Authentication &copy) {
	std::cout << "Authentication assignation operator called" << std::endl;
	if (this != &copy)
	{
		this->_registeredUsers = copy._registeredUsers;
		this->_authenticatedClients = copy._authenticatedClients;
	}
	return *this;
}

Authentication::~Authentication() {
	std::cout << "Authentication destructor called" << std::endl;
}

// Getters

std::map<std::string, std::string> Authentication::getRegisteredUsers(void) const {
	return _registeredUsers;
}

std::map<int, std::string> Authentication::getAuthenticatedClients(void) const {
	return _authenticatedClients;
}


// Setters

void Authentication::setRegisteredUsers(std::map<std::string, std::string> registeredUsers){
	_registeredUsers = registeredUsers;
}

void Authentication::setAuthenticatedClients(std::map<int, std::string> authenticatedClients){
	_authenticatedClients = authenticatedClients;
}

