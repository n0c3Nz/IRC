#ifndef ERRORHANDLER_HPP
#define ERRORHANDLER_HPP

#include "Includes.hpp"

class ErrorHandler : public std::exception {
	private:
    	std::string _message;
	public:
		// Constructor que acepta un mensaje de error personalizado
		explicit ErrorHandler(const std::string& message) : _message(message) {}

		// Sobrecarga del método what() de std::exception para devolver el mensaje de error
		virtual const char* what() const noexcept {
			return _message.c_str();
		}

		// Métodos estáticos para lanzar excepciones específicas
		static ErrorHandler SocketCreation() {
			return ErrorHandler("Error: Unable to create socket.");
		}

		static ErrorHandler SocketBind() {
			return ErrorHandler("Error: Unable to bind socket.");
		}

		static ErrorHandler SocketListen() {
			return ErrorHandler("Error: Unable to listen on socket.");
		}

		static ErrorHandler SocketAccept() {
			return ErrorHandler("Error: Unable to accept connection.");
		}

		static ErrorHandler SocketEpoll() {
			return ErrorHandler("Error: Unable to create Epoll socket.");
		}

		static ErrorHandler GetPeerName() {
			return ErrorHandler("Unknown");
		}
};


#endif