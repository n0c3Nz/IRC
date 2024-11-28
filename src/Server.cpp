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

		if (listen(this->_socket, MAX_CLIENTS) == -1) {
			throw ErrorHandler::SocketListen();
		}

		std::cout << "Server listening on port " << this->_port << std::endl;
	}
	catch (std::exception &e){
		std::cerr << e.what() << std::endl;
		exit(1);
	}
}

void Server::run(void) {
    // Usar nuestro socket para aceptar conexiones en una poll
    struct pollfd fds[MAX_CLIENTS + 1];
    fds[0].fd = _socket;
    fds[0].events = POLLIN;

    while (true)
    {
        int ret = poll(fds, MAX_CLIENTS + 1, 0); // Cambiamos 1 por MAX_CLIENTS + 1 para revisar todos los sockets
        //int epfd = epoll_create(1);//NUEVO
        //struct epoll_event ev, events[1];//NUEVO
        //ev.events = EPOLLIN;//NUEVO
        //ev.data.fd = _socket;//NUEVO
        //epoll_ctl(epfd, EPOLL_CTL_ADD, _socket, &ev);//NUEVO 
        try {
            if (ret > 0)
            {
                // Verificar si hay una nueva conexión en el socket maestro
                if (fds[0].revents & POLLIN)
                {
                    // Crear nuevo socket y añadirlo a _clients
                    int new_socket = accept(_socket, NULL, NULL);
                    if (new_socket == -1)
                        throw ErrorHandler::SocketAccept();
                    std::cout << "Nueva conexión aceptada." << std::endl;
                    send(new_socket, "Bienvenido al servidor de chat\n", std::strlen("Bienvenido al servidor de chat\n"), 0);
                    send(new_socket, "Por favor, selecciona un username con /USERNAME para continuar\n", std::strlen("Por favor, selecciona un username con /USERNAME para continuar\n"), 0);
                    // Añadir nuevo socket a poll y a _clients pero en orden
                    int i = 1;
                    while (i < MAX_CLIENTS + 1 && _clients.find(i) != _clients.end()) {
                        i++;
                    }
                    if (i == MAX_CLIENTS + 1)
                    {
                        close(new_socket);
                        std::cerr << "Demasiados clientes conectados" << std::endl;
                        exit(1);
                    }
                    fds[i].fd = new_socket;
                    fds[i].events = POLLIN;
                    _clients[i] = Client(new_socket);
                    std::cout << "Nuevo cliente añadido: " << _clients[i].getNickname() << std::endl;
                }

                // Recibir mensajes de los clientes
                for (int i = 1; i < MAX_CLIENTS + 1; i++) {
                    if (fds[i].fd != -1 && (fds[i].revents & POLLIN)) {
                        // Leer datos del cliente
                        char buffer[MAX_MSG_SIZE] = {0};
                        int valread = read(fds[i].fd, buffer, sizeof(buffer) - 1);
                        if (valread > 0) {
                            buffer[valread] = '\0'; // Asegurar que el mensaje sea una cadena terminada en nulo
                            if (std::string(buffer) == "/USERNAME\n") {
                                std::cout << "[ COMANDO DETECTADO ]" << std::endl;
                                send(fds[i].fd, "Escribe tu nombre\n", std::strlen("Escribe tu nombre\n"), 0);
                            }
                            std::cout << "Mensaje recibido del cliente " << i << ": " << buffer << std::endl;
                            // Aquí podrías agregar lógica para manejar los comandos o mensajes del cliente
                        }
                        else {
                            // Si la lectura falla o el cliente cerró la conexión
                            std::cout << "Cliente " << i << " desconectado." << std::endl;
                            close(fds[i].fd);
                            fds[i].fd = -1;
                            _clients.erase(i); // Eliminar el cliente de la lista
                        }
                    }
                }
            }
        }
        catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
            exit(1);
        }
    }
    close(_socket);
}


