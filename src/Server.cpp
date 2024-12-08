#include <Server.hpp>


Server::Server(){
	this->_port = 6660;
	this->_socket = -1;
	_servAddr.sin_family = AF_INET;//IPv4
	_servAddr.sin_addr.s_addr = INADDR_ANY;// Para escuchar en todas las interfaces
	_servAddr.sin_port = htons(this->_port);
    _password = "1234";
    _motd = "Welcome to the Anon Chat Server";
}

Server::Server(int port, std::string password, std::string motd){
	this->_port = port;
	this->_socket = -1;
	_servAddr.sin_family = AF_INET;//IPv4
	_servAddr.sin_addr.s_addr = INADDR_ANY;// Para escuchar en todas las interfaces
	_servAddr.sin_port = htons(this->_port);
    _password = password;
    _motd = motd;
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

Server::~Server() {
    close(this->_socket);
    _clients.clear(); // `clear` llama automáticamente al destructor de `std::shared_ptr`.
    _channels.clear(); // Si también se usa un contenedor que maneja memoria, se liberará correctamente.
    std::cout << "Server closed" << std::endl;
}


// Getters

int Server::getPort(void) const{
	return this->_port;
}

int Server::getSocket(void) const{
	return this->_socket;
}

const std::map<int, std::shared_ptr<Client>>& Server::getClients() const {
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

void Server::setClients(std::map<int, std::shared_ptr<Client>> clients) {
    this->_clients = std::move(clients);
}


void Server::setChannels(std::map<std::string, Channel> channels){
	this->_channels = channels;
}

void Server::setNickname(int clientFd, const std::string &nickname) {
    // Primero recorremos todos los clientes para comprobar si el nickname ya está en uso
    for (std::map<int, std::shared_ptr<Client>>::iterator iter = _clients.begin(); iter != _clients.end(); ++iter) {
        // Si encontramos el nickname en uso en un cliente distinto del actual (clientFd)
        if (iter->second->getNickname() == nickname && iter->first != clientFd) {
            std::cerr << "[DEBUG] El nick '" << nickname << "' ya está siendo usado por el cliente con fd: " 
                      << iter->first << std::endl;
            std::string response = "ERROR :Nickname en uso\r\n";
            send(clientFd, response.c_str(), response.size(), 0);
            return;
        }
    }

    // Si llegamos hasta aquí, el nickname no está en uso.
    // Si el cliente existe, simplemente le asignamos el nuevo nickname.
        _clients[clientFd]->setNickname(nickname);
        std::cerr << "[DEBUG] Nombre establecido para nuevo cliente: " << nickname << std::endl;
        //send(clientFd, "NICK :Nickname establecido\r\n", 29, 0);
        return;
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

void Server::handshake(int clientFd){
    std::string response;
    response = ":" SRV_NAME " RPL_WELCOME  " "c3nz :Welcome to the IRC Network c3nz\r\n";
    send(clientFd, response.c_str(), response.size(), 0);
    response = ":" SRV_NAME " RPL_YOURHOST " "c3nz :Your host is c3nz, running version " SRV_VERSION "\r\n";
    send(clientFd, response.c_str(), response.size(), 0);
    response = ":" SRV_NAME " RPL_CREATED " "c3nz :This server" SRV_NAME " was created " __DATE__ " " __TIME__ "\r\n";
    send(clientFd, response.c_str(), response.size(), 0);
    response = ":" SRV_NAME " RPL_MYINFO " "c3nz :This server" SRV_NAME " " SRV_VERSION " Gikl OV" "\r\n";
    send(clientFd, response.c_str(), response.size(), 0);
}

void Server::quit(int clientFd){
    std::string response;
    response = "QUIT :Client disconnected por la gloria de mi padre\r\n";
    send(clientFd, response.c_str(), response.size(), 0);
    epoll_ctl(_epollFd, EPOLL_CTL_DEL, clientFd, NULL);
    close(clientFd);
    _clients.erase(clientFd);
    std::cout << "Cliente desconectado" << std::endl;
}

void Server::nick(int clientFd, std::string nickname){
    setNickname(clientFd, nickname);
}

void Server::user(int clientFd, std::string username, std::string realname){
    _clients[clientFd]->setUsername(username);
    _clients[clientFd]->setRealname(realname);
}

int checkEmptyAndAlnum(std::string str){
    if (str.empty()){
        return 0;
    }
    for (int i = 0; i < str.length(); i++){
        if (!isalnum(str[i]) || str[i] == ' '){
            return 0;
        }
    }
    return 1;
}

void Server::processCommand(int clientFd, std::string command) {
    std::string response;
    if (command == "QUIT") {
        quit(clientFd);
        return;
    }else if (std::strncmp(command.c_str(), "PASS ", 5) == 0){
        std::cout << "[LOG] COMMAND: PASS DETECTADO" << std::endl;
        std::string password = command.substr(5);
        if (password.empty()){
            response = "ERROR :Password vacío\r\n";
            send(clientFd, response.c_str(), response.size(), 0);
            return;
        }
        if (password == _password){
            _clients[clientFd]->setPwdSent();
        }
        else{
            response = "ERROR :Password incorrecto\r\n";
            send(clientFd, response.c_str(), response.size(), 0);
            return;
        }
        //return;
    }else if (std::strncmp(command.c_str(), "NICK ", 5) == 0 && _clients[clientFd]->getPwdSent()){
        std::cout << "[LOG] COMMAND: NICK DETECTADO" << std::endl;
        std::string nickname = command.substr(5);
        if (!checkEmptyAndAlnum(nickname)){
            response = "ERROR :Invalid nickname\r\n";
            send(clientFd, response.c_str(), response.size(), 0);
            return;
        }
        nick(clientFd, nickname);
        //return;
    } else if (std::strncmp(command.c_str(), "USER ", 5) == 0 && _clients[clientFd]->getPwdSent()){
        std::cout << "[LOG] COMMAND: USER DETECTADO" << std::endl;
        // Guardar la posicion del espacio desde command[5], es decir, el segundo espacio
        int pos = command.find(' ', 5);
        std::string username = command.substr(5, pos - 5);
        pos = command.find(':');
        std::string realname = command.substr(pos + 1);
        if (!checkEmptyAndAlnum(username) || !checkEmptyAndAlnum(realname)){
            response = "ERROR :Invalid username or realname\r\n";
            send(clientFd, response.c_str(), response.size(), 0);
            return;
        }
        user(clientFd, username, realname);
        _clients[clientFd]->setIsAuth();//falta añadirlo a la lista de authenticatedClients
        std::string hash = _clients[clientFd]->getNickname() + ":" + _clients[clientFd]->getUsername() + ":" + _clients[clientFd]->getRealname();
        handshake(clientFd);
        _authenticatedClients.push_back(hash);
        std::cerr << "[DEBUG] Hash añadido: " << hash << std::endl;//falta añadirlo a la lista de authenticatedClients
        //return;
    } else {
        response = "ERROR :Unknown command ma G\r\n";
    }
    //for tests print client nickname, username and realname
    std::cout << "Nickname: " << _clients[clientFd]->getNickname() << std::endl;
    std::cout << "Username: " << _clients[clientFd]->getUsername() << std::endl;
    std::cout << "Realname: " << _clients[clientFd]->getRealname() << std::endl;
}

void Server::handleClientData(int clientFd) {
    char buffer[MAX_MSG_SIZE];
    bzero(buffer, MAX_MSG_SIZE);

    // Leer datos del cliente
    int bytesRead = recv(clientFd, buffer, MAX_MSG_SIZE - 1, 0);

    if (bytesRead == 0) {
        // Cliente cerró la conexión
        std::cerr << "[DEBUG] Cliente cerró la conexión: " << clientFd << std::endl;
        epoll_ctl(_epollFd, EPOLL_CTL_DEL, clientFd, NULL);
        close(clientFd);
        _clients.erase(clientFd);
        return;
    }

    if (bytesRead < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // No hay datos por ahora
            std::cerr << "[DEBUG] recv devolvió EAGAIN/EWOULDBLOCK para cliente: " << clientFd << std::endl;
            return;
        } else {
            // Error crítico, desconectar al cliente
            std::cerr << "[DEBUG] Error crítico en recv (cliente " << clientFd << "): " << strerror(errno) << std::endl;
            epoll_ctl(_epollFd, EPOLL_CTL_DEL, clientFd, NULL);
            close(clientFd);
            _clients.erase(clientFd);
            return;
        }
    }

    // Validar existencia del cliente antes de procesar
    std::map<int, std::shared_ptr<Client>>::iterator it = _clients.find(clientFd);
    if (it == _clients.end()) {
        std::cerr << "[DEBUG] Cliente no encontrado en el mapa (fd: " << clientFd << ")." << std::endl;
        return;
    }

    // Acceder al buffer del cliente desde std::shared_ptr
    try {
        std::shared_ptr<Client> client = it->second;
        std::cerr << "[DEBUG] Acumulando datos para cliente " << clientFd << ": tamaño del buffer previo: " << client->_buffer.size() << std::endl;

        client->_buffer.append(buffer, bytesRead);
        std::cerr << "[DEBUG] Datos acumulados en el buffer del cliente " << clientFd << ": " << client->_buffer << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "[DEBUG] Error al acumular datos en el buffer: " << e.what() << std::endl;
        return;
    }

    // Procesar comandos completos
    size_t pos;
    while ((pos = it->second->_buffer.find('\n')) != std::string::npos) {
        try {
            // Extraer comando completo
            std::string command = it->second->_buffer.substr(0, pos);
            it->second->_buffer.erase(0, pos + 1);

            // Ignorar comandos vacíos
            if (!command.empty()) {
                std::cerr << "[DEBUG] Procesando comando del cliente " << clientFd << ": " << command << std::endl;
                processCommand(clientFd, command);
            }
        } catch (const std::exception &e) {
            std::cerr << "[DEBUG] Error procesando comando: " << e.what() << std::endl;
            return;
        }
    }

    // Depurar: Mostrar buffer restante después del procesamiento
    std::cerr << "[DEBUG] Buffer restante para cliente " << clientFd << ": " << it->second->_buffer << std::endl;
}



void Server::run(void) {
    int clientFd;
    _epollFd = epoll_create(MAX_EVENTS);// Crea el file descriptor para el epoll
    struct epoll_event event, events[MAX_EVENTS];// Estructuras para manejar eventos
    // Agregar el descriptor del servidor al epoll
    event.events = EPOLLIN;      // Interesado en eventos de lectura
    event.data.fd = _socket;   // Asociar con el descriptor del servidor
        if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, _socket, &event) == -1) {
        std::cerr << "Error al agregar descriptor al epoll" << std::endl;
        close(_socket);
        close(_epollFd);
        return;
    }

    while (true)
    {
        int num_events = epoll_wait(_epollFd, events, MAX_EVENTS, -1);// Creamos un array de eventos y esperamos a que ocurran
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
                if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientFd, &event) == -1) {// Agregamos el socket del cliente al epoll
                    std::cerr << "Error al agregar cliente al epoll" << std::endl;
                    close(clientFd);
                    continue;
                }
                _clients[clientFd] = std::make_shared<Client>(clientFd);//_clients[clientFd] = Client(clientFd);// Agregamos el cliente al mapa de clientes
                AnnounceConnection(clientFd);// Anunciamos la conexión del cliente
            } else {
                // Cliente existente
                int clientFd = events[i].data.fd;// Obtenemos el socket del cliente existente que generó el evento
                handleClientData(clientFd);// Procesamos los datos del cliente
            }
        }
    }
    close(_epollFd);
    return;
}