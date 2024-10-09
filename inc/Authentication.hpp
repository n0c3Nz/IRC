#ifndef AUTHENTICATION_HPP
#define AUTHENTICATION_HPP

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

class Authentication {
	private:
		std::map<std::string, std::string> _registeredUsers;//: Mapa de usuarios registrados y sus contraseñas.
		std::map<int, std::string> _authenticatedClients;//: Mapa de clientes autenticados por socket. / Peter : Password
	public:
		Authentication();
		Authentication(const Authentication &copy);
		Authentication &operator=(const Authentication &copy);
		~Authentication();
		// Getters
		std::map<std::string, std::string> getRegisteredUsers(void) const;
		std::map<int, std::string> getAuthenticatedClients(void) const;
		// Setters
		void setRegisteredUsers(std::map<std::string, std::string> registeredUsers);
		void setAuthenticatedClients(std::map<int, std::string> authenticatedClients);
};

#endif