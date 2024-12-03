#include <Server.hpp>


Server::Server(){
	this->_port = 6660;
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
	close(this->_socket);

	std::map<int, Client>::iterator it = this->_clients.begin();
	while (it != this->_clients.end()){
		close(it->first);
		it++;
	}

	std::map<std::string, Channel>::iterator it2 = this->_channels.begin();
	while (it2 != this->_channels.end()){
		it2++;
	}

	std::cout << "Server closed" << std::endl;
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

void Server::AnnounceConnection(int clientFd) const{
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    getpeername(clientFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
    std::cout << "Connection from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;
}

void Server::start(void) {
    std::cout << "Server started" << std::endl;
    this->_socket = socket(AF_INET, SOCK_STREAM, 0);
    try{
		if (this->_socket == -1) {
			throw ErrorHandler::SocketCreation();
		}

		if (bind(this->_socket, (struct sockaddr *)&_servAddr, sizeof(_servAddr)) == -1) {
			throw ErrorHandler::SocketBind();
		}

		if (listen(this->_socket, MAX_EVENTS) == -1) {
			throw ErrorHandler::SocketListen();
		}

		std::cout << "Server listening on port " << this->_port << std::endl;
	}
	catch (std::exception &e){
		std::cerr << e.what() << std::endl;
		exit(1);
	}
}

void setNonBlocking(int socketFd) {
    int flags = fcntl(socketFd, F_GETFL, 0);// Obtiene los flags actuales del socket
    fcntl(socketFd, F_SETFL, flags | O_NONBLOCK);// Agrega el flag O_NONBLOCK, F_SETFL establece los flags del socket
}

void Server::run(void) {
    int clientFd;
    int epollFd = epoll_create(MAX_EVENTS);// Crea el file descriptor para el epoll
    struct epoll_event event, events[MAX_EVENTS];// Estructuras para manejar eventos
    // Agregar el descriptor del servidor al epoll
    event.events = EPOLLIN;      // Interesado en eventos de lectura
    event.data.fd = _socket;   // Asociar con el descriptor del servidor
        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, _socket, &event) == -1) {
        std::cerr << "Error al agregar descriptor al epoll" << std::endl;
        close(_socket);
        close(epollFd);
        return;
    }

    while (true)
    {
        int num_events = epoll_wait(epollFd, events, MAX_EVENTS, -1);// Creamos un array de eventos y esperamos a que ocurran
        try{
            if (num_events == -1) {// Si hay un error en epoll_wait lanzamos una excepción
                throw ErrorHandler::SocketEpoll();
            }
        }
        catch (std::exception &e){
            std::cerr << e.what() << std::endl;
            return;
        }
        for (int i = 0; i < num_events; i++) {
            if (events[i].data.fd == _socket) {// Si el evento es del servidor significa que hay un nuevo cliente
                // Nuevo cliente
                socklen_t addrlen = sizeof(_servAddr);// Tamaño de la dirección del servidor
                int clientFd = accept(_socket, (struct sockaddr *)&_servAddr, &addrlen);//socket del nuevo cliente
                if (clientFd == -1) {
                    std::cerr << "Error aceptando conexión" << std::endl;
                    continue;
                }
                setNonBlocking(clientFd);// Hacemos el socket del cliente no bloqueante
                event.events = EPOLLIN | EPOLLET; // Edge-Triggered
                event.data.fd = clientFd;
                if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &event) == -1) {// Agregamos el socket del cliente al epoll
                    std::cerr << "Error al agregar cliente al epoll" << std::endl;
                    close(clientFd);
                    continue;
                }
                _clients[clientFd] = Client(clientFd);// Agregamos el cliente al mapa de clientes
                AnnounceConnection(clientFd);// Anunciamos la conexión del cliente
            } else {
                // Cliente existente
                int clientFd = events[i].data.fd;// Obtenemos el socket del cliente existente que generó el evento
                char buffer[MAX_MSG_SIZE];
                bzero(buffer, MAX_MSG_SIZE);
                int bytesRead = recv(clientFd, buffer, MAX_MSG_SIZE, 0);
                if (bytesRead <= 0) {// Si no se reciben bytes significa que el cliente se desconectó
                    // Cliente desconectado
                    epoll_ctl(epollFd, EPOLL_CTL_DEL, clientFd, NULL);// Eliminamos el socket del cliente del epoll
                    close(clientFd);// Cerramos el socket del cliente
                    _clients.erase(clientFd);// Eliminamos al cliente del mapa de clientes
                    std::cout << "Cliente desconectado" << std::endl;
                } else {
                    buffer[bytesRead] = '\0';
                    std::cout << "Mensaje recibido: " << buffer << std::endl;
                    if (strcmp(buffer, "exit\n") == 0) {
                        send(clientFd, "Disconnecting ...", strlen("Disconnecting ..."), 0);
                        epoll_ctl(epollFd, EPOLL_CTL_DEL, clientFd, NULL);
                        close(clientFd);
                        _clients.erase(clientFd);
                        std::cout << "Cliente desconectado" << std::endl;
                    } else {
                        // Responder al cliente
                        send(clientFd, buffer, strlen(buffer), 0);
                    }
                }
            }
        }

    }
    close(epollFd);
    return;
}