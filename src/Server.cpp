#include <Server.hpp>


Server::Server(){
	this->_port = 6660;
	this->_socket = -1;
	_servAddr.sin_family = AF_INET;//IPv4
	_servAddr.sin_addr.s_addr = INADDR_ANY;// Para escuchar en todas las interfaces
	_servAddr.sin_port = htons(this->_port);
    _password = "1234";
}

Server::Server(int port, std::string password, std::string motd){
	this->_port = port;
	this->_socket = -1;
	_servAddr.sin_family = AF_INET;//IPv4
	_servAddr.sin_addr.s_addr = INADDR_ANY;// Para escuchar en todas las interfaces
	_servAddr.sin_port = htons(this->_port);
    _password = password;
}

Server::Server(const Server &copy){
	*this = copy;
}

Server &Server::operator=(const Server &copy){
	if (this != &copy){
		this->_port = copy._port;
		this->_socket = copy._socket;
		this->_clients = copy._clients;
		//vector of channels

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


const std::vector <Channel>& Server::getChannels() const {
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


void Server::setChannels(std::vector <Channel> channels) {
    this->_channels = std::move(channels);
}

void Server::setNickname(int clientFd, const std::string &nickname) {
    _clients[clientFd]->setNickname(nickname);
    std::cerr << "[DEBUG] Nombre establecido para el nuevo cliente: " << nickname << std::endl;
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
    response = ":" SRV_NAME " " RPL_WELCOME " " + _clients[clientFd]->getNickname() + " :Welcome to the IRC Network " + _clients[clientFd]->getNickname() + "\r\n";
    send(clientFd, response.c_str(), response.size(), 0);
    response = ":" SRV_NAME " " RPL_YOURHOST " " + _clients[clientFd]->getNickname() + " :Your host is c3nz, running version " SRV_VERSION "\r\n";
    send(clientFd, response.c_str(), response.size(), 0);
    response = ":" SRV_NAME " " RPL_CREATED " " + _clients[clientFd]->getNickname() + " :This server" SRV_NAME " was created " __DATE__ " " __TIME__ "\r\n";
    send(clientFd, response.c_str(), response.size(), 0);
    response = ":" SRV_NAME " " RPL_MYINFO " " + _clients[clientFd]->getNickname() + " :This server" SRV_NAME " " SRV_VERSION " Gikl OV" "\r\n";
    send(clientFd, response.c_str(), response.size(), 0);
    motd(clientFd);
}
// Comandos
void Server::quit(int clientFd, std::string message){
    std::string response;
    //Tengo que enviar un quit a todos los canales a los que pertenece el cliente
    for (size_t i = 0; i < _clients[clientFd]->getJoinedChannels().size(); i++){
        //:<nick>!<user>@<host> QUIT :<razón>
        std::string notification = ":" + _clients[clientFd]->getNickname() + "!" + _clients[clientFd]->getUsername() + "@" + _clients[clientFd]->getHost() + " QUIT :" + message + "\r\n";
        for (size_t j = 0; j < _channels.size(); j++){
            if (_channels[j].getName() == _clients[clientFd]->getJoinedChannels()[i]){
                for (size_t k = 0; k < _channels[j].getMembers().size(); k++){
                    send(_channels[j].getMembers()[k].getSocket(), notification.c_str(), notification.size(), 0);
                }
            }
        }
    }
    //Tengo que eliminar al cliente de los canales a los que pertenece
    for (size_t i = 0; i < _clients[clientFd]->getJoinedChannels().size(); i++){
        for (size_t j = 0; j < _channels.size(); j++){
            if (_channels[j].getName() == _clients[clientFd]->getJoinedChannels()[i]){
                _channels[j].removeClient(*_clients[clientFd]);
            }
        }
    }
    std::cout << "Cliente desconectado" << std::endl;
    closeConnection(clientFd);
}

void Server::nick(int clientFd, std::string nickname){
    setNickname(clientFd, nickname);
}

void Server::user(int clientFd, std::string username, std::string realname){
    _clients[clientFd]->setUsername(username);
    _clients[clientFd]->setRealname(realname);
}

void Server::motd(int clientFd){
    std::string response;
    std::string nickname = _clients[clientFd]->getNickname();
    std::cerr << "[DEBUG] Enviando MOTD a " << nickname << std::endl;

    response = ":" SRV_NAME " " RPL_MOTDSTART " :- " SRV_NAME " Message of the Day - \r\n";
    send(clientFd, response.c_str(), response.size(), 0);
    response = ":" SRV_NAME " " RPL_MOTD " ---------------------------- \r\n";
    send(clientFd, response.c_str(), response.size(), 0);
    response = ":" SRV_NAME " " RPL_MOTD " " + nickname + " Bienvenido a " SRV_NAME " " + nickname + "\r\n";
    send(clientFd, response.c_str(), response.size(), 0);
    response = ":" SRV_NAME " " RPL_MOTD " ---------------------------- \r\n";
    send(clientFd, response.c_str(), response.size(), 0);
    response = ":" SRV_NAME " " RPL_MOTD " Las reglas son sencillas: \r\n";
    send(clientFd, response.c_str(), response.size(), 0);
    response = ":" SRV_NAME " " RPL_MOTD " - Honrarás a los admins sobre todas las cosas. \r\n";
    send(clientFd, response.c_str(), response.size(), 0);
    response = ":" SRV_NAME " " RPL_MOTD " - No dirás palabrotas. \r\n";
    send(clientFd, response.c_str(), response.size(), 0);
    response = ":" SRV_NAME " " RPL_MOTD " - Si lees esto me debes un BTC. \r\n";
    send(clientFd, response.c_str(), response.size(), 0);
    response = ":" SRV_NAME " " RPL_ENDOFMOTD " " + nickname + " :End of /MOTD command.\r\n";
    send(clientFd, response.c_str(), response.size(), 0);
}

void Server::names(int clientFd, std::string channelName){
    std::string response;
    if (channelName.empty()){
            response = "ERROR :No channel specified\r\n";
            send(clientFd, response.c_str(), response.size(), 0);
            return;
    }
    if (checkChannelExistence(clientFd, channelName) || checkChannelMembership(clientFd, channelName))
        return;
    std::string allMembers;
    //preparar un string con los nicks del canal separados por espacios
    for (size_t i = 0; i < _channels.size(); i++){
        if (_channels[i].getName() == channelName){
            for (size_t j = 0; j < _channels[i].getMembers().size(); j++){
                //aqui es donde tengo que detectar si es operador para añaddirle el @
                if (_channels[i].isOperator(_channels[i].getMembers()[j].getNickname())){
                    allMembers += "@" + _channels[i].getMembers()[j].getNickname() + " ";
                }else{
                    allMembers += _channels[i].getMembers()[j].getNickname() + " ";
                }
            }
        }
    }
    response = ":" SRV_NAME " " RPL_NAMREPLY " " + _clients[clientFd]->getNickname() + " = " + channelName + " :" + allMembers + "\r\n";
    send(clientFd, response.c_str(), response.size(), 0);
    response = ":" SRV_NAME " " RPL_ENDOFNAMES " " + _clients[clientFd]->getNickname() + " " + channelName + " :End of /NAMES list\r\n";
    send(clientFd, response.c_str(), response.size(), 0);
}

// -------------------

int checkEmptyAndAlnum(std::string str){
    if (str.empty()){
        return 1;
    }
    for (int i = 0; i < str.length(); i++){
        if (!isalnum(str[i]) && str[i] != ' ' && str[i] != '\r' && str[i] != '\n'){
            return 1;
        }
    }
    return 0;
}

void deleteCarriageReturn(std::string &str){
    if (str[str.length() - 1] == '\r'){
        str.pop_back();
    }
}

void Server::closeConnection(int clientFd){
    epoll_ctl(_epollFd, EPOLL_CTL_DEL, clientFd, NULL);
    close(clientFd);
    _clients.erase(clientFd);
}

void Server::checkOnline(int clientFd){
    std::string nickname = _clients[clientFd]->getNickname();
    // Primero recorremos todos los clientes para comprobar si el nickname ya está en uso
    for (std::map<int, std::shared_ptr<Client>>::iterator iter = _clients.begin(); iter != _clients.end(); ++iter) {
        // Si encontramos el nickname en uso en un cliente distinto del actual (clientFd)
        if (iter->second->getNickname() == nickname && iter->first != clientFd) {
            std::cerr << "[DEBUG] El usuario '" << nickname << "' ha abierto una nueva conexión con el cliente con fd: " 
                      << iter->first << std::endl;
            //cerrar la conexión anterior y dejar la nueva
            closeConnection(iter->first);
            return;
        }
    }
}

int Server::checkHash(int clientFd){
    //Comprobar si existe algún hash cuyo nickname sea igual al del cliente
    for (int i = 0; i < _authenticatedClients.size(); i++){
        std::string hash = _authenticatedClients[i];
        std::string nickname = hash.substr(0, hash.find(':'));
        if (nickname == _clients[clientFd]->getNickname()){
            if (_clients[clientFd]->getHash() == hash){
                //Comprobar si hay otro cliente conectado y autenticado con el mismo nickname, si lo hay desconectarlo
                checkOnline(clientFd);
                return 0;
            }
            //Manejar el caso de que el nick pertenezca a otro cliente
            else{
                std::string response = "ERROR :This nickname is already taken\r\n";
                send(clientFd, response.c_str(), response.size(), 0);
                return 1;
            }
        }
    }
    _authenticatedClients.push_back(_clients[clientFd]->getHash());// Lo insertamos en la lista de clientes autenticados
    return 0;
}


	
int		Server::authenticateChannel(const Channel &channel, const std::string &password) const
{
	if (channel.getPwd() == password)
		return 0;
	return 1;
}

void	Server::joinChannelServerSide(std::map<std::string, std::string> channelKey, int clientFd)
{

	// channel.addClient(*_clients[clientFd]);
	// _clients[clientFd]->joinChannel(channel);
	for (std::map<std::string, std::string>::iterator it = channelKey.begin(); it != channelKey.end(); ++it)
	{
		std::string channelName = it->first;
		std::string provideKey = it->second;
		
		Channel *channel = nullptr;
		if (exist(channelName) == 1) //no existe el canal
		{
			_channels.push_back(Channel(channelName));
			channel = &_channels.back();
			channel->addClient(*_clients[clientFd]);
			_clients[clientFd]->joinChannel(*channel);
			std::cerr << "[DEBUG] CANNAL NO EXISTENTE: "<< channelName << std::endl;
			sendConfirmJoin(clientFd, channelName);
		}
		else//existe
		{
			for (size_t i = 0; i < _channels.size(); i++) {
            	if (_channels[i].getName() == channelName) {
                	channel = &_channels[i];
                	break;
           		}
        	}
			if (exist(channelName) == 3)
			{
				if (authenticateChannel(*channel, channelName) == 0)
					{
						channel->addClient(*_clients[clientFd]);
						_clients[clientFd]->joinChannel(*channel);
					}
				else
				{
					//enviar codigo de error de autenticacion de credenciales
					std::string errorMsg = ":" SRV_NAME " " ERR_BADCHANNELKEY " " + _clients[clientFd]->getNickname() + " " + channelName + " :Cannot join channel (+k) - bad key\r\n";
				}
			}
			if (exist(channelName) == 2)
			{
				channel->addClient(*_clients[clientFd]);
				_clients[clientFd]->joinChannel(*channel);
			}
			sendConfirmJoin(clientFd, channelName);
		}
	}
}

void	Server::sendPrivateMessage(std::string senderNick, std::string msg, std::string receiverNick)
{ 
	// for (std::map<int, std::shared_ptr<Client>>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	// 	std::cerr << "[DEBUG] fd: " << it->second->getSocket() << " Nick: " << it->second->getNickname() << std::endl;
	// std::cerr << "[DEBUG] MENSAJE DETECTADO: " << msg << std::endl;
	int senderFd = findUserByNick(senderNick);
	int	receiverFd = findUserByNick(receiverNick);
	std::cerr << "[DEBUG] SENDER FD: " << senderFd << "RECEIVER FD: " << receiverFd << std::endl;
	std::string fullMsg = ':' + senderNick + " PRIVMSG " + receiverNick + " :" + msg + "\r\n";
	if (senderFd != receiverFd  && receiverFd != -1){
	 	send(receiverFd, fullMsg.c_str(), fullMsg.size(), 0);
        return;
    } else {
        std::string response = ":" SRV_NAME " " ERR_NOSUCHNICK " " + senderNick + " " + receiverNick + " :No such nick/channel\r\n";
        send(senderFd, response.c_str(), response.size(), 0);
    }
}

int	Server::findUserByNick(const std::string &nick)
{
	for (std::map<int, std::shared_ptr<Client>>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		if (it->second->getNickname() == nick)
			return it->second->getSocket();
	return (-1);
}

void	Server::sendChannelMessage(int clientFd, const std::string &msg, const std::string &channelName)
{
    std::string senderNick = _clients[clientFd]->getNickname();
    std::string host = _clients[clientFd]->getHost();
	for (size_t i = 0; i < this->_channels.size(); i++)
		if (_channels[i].getName() == channelName)
		{
			std::cerr << "[DEBUG] CANALES: " << this->_channels[i].getName() << std::endl;	
			this->_channels[i].sendMsg(senderNick, host, msg);
		}
}

void Server::sendConfirmJoin(int clientFd, const std::string &channelName){
    std::string response;
    std::string allMembers;
    //preparar un string con los nicks del canal separados por espacios
    for (size_t i = 0; i < _channels.size(); i++){
        if (_channels[i].getName() == channelName){
            for (size_t j = 0; j < _channels[i].getMembers().size(); j++){
                //aqui es donde tengo que detectar si es operador para añaddirle el @
                if (_channels[i].isOperator(_channels[i].getMembers()[j].getNickname())){
                    allMembers += "@" + _channels[i].getMembers()[j].getNickname() + " ";
                }else{
                    allMembers += _channels[i].getMembers()[j].getNickname() + " ";
                }
            }
        }
    }

    //Enviar la notificación a los demás miembros del canal de que un nuevo usuario se ha unido y al propio usuario
    for (size_t i = 0; i < _channels.size(); i++){
        if (_channels[i].getName() == channelName){
            std::string notification = ":" + _clients[clientFd]->getNickname() + " JOIN " + channelName + "\r\n";
            for (size_t j = 0; j < _channels[i].getMembers().size(); j++){
                    std::cerr << "[DEBUG] Enviando notificación: " << notification << std::endl;
                    send(_channels[i].getMembers()[j].getSocket(), notification.c_str(), notification.size(), 0);
            }
        }
    }
    //Topic
    response = ":" SRV_NAME " " RPL_TOPIC " " + _clients[clientFd]->getNickname() + " " + channelName + " :No topic is set\r\n";
    send(clientFd, response.c_str(), response.size(), 0);
    response = ":" SRV_NAME " " RPL_NAMREPLY " " + _clients[clientFd]->getNickname() + " = " + channelName + " :" + allMembers + "\r\n";
    send(clientFd, response.c_str(), response.size(), 0);
    response = ":" SRV_NAME " " RPL_ENDOFNAMES " " + _clients[clientFd]->getNickname() + " " + channelName + " :End of /NAMES list\r\n";
    send(clientFd, response.c_str(), response.size(), 0);
}

int	Server::exist(const std::string &channelName) const
{
	for (size_t i = 0; i < this->_channels.size(); i++){
		if (std::strncmp(this->_channels[i].getName().c_str(), channelName.c_str(), channelName.length()) == 0)
		{
            if (this->_channels[i].getIfPwd() == true)
				return 3;
			else
				return 2;
		}
    }
	return 1;
}

std::map<std::string, std::string>		Server::parseJoinRequets(std::string request) const
{
	std::map<std::string, std::string> channelKeys;
	std::vector<std::string> channels;
	std::vector<std::string> keys;
	size_t	spacePos = request.find(' ');
	
	std::string channelsPart = request.substr(0, spacePos);
	std::string keysPart = (spacePos != std::string::npos) ? request.substr(spacePos + 1) : "";

	std::istringstream channelStream(channelsPart);
	std::string channel;
	while (std::getline(channelStream, channel, ','))
		channels.push_back(channel);

	if (!keysPart.empty())
	{
		std::istringstream keyStream(keysPart);
		std::string key;
		while (std::getline(keyStream, key, ','))
			keys.push_back(key);
	}

	for (size_t i = 0; i < channels.size(); i++)
	{
		if (i < keys.size())
		{
			if (exist(channels[i]) == 3)
				channelKeys[channels[i]] = keys[i];
			else if (exist(channels[i]) == 2)
				channelKeys[channels[i]] = "";
			else if (exist(channels[i]) == 1)
				channelKeys[channels[i]] = "";
		}
		else
			channelKeys[channels[i]] = "";
	}
	return channelKeys;
}

int Server::checkChannelExistence(int clientFd, const std::string &channelName){
    for (size_t i = 0; i < _channels.size(); i++){
        if (_channels[i].getName() == channelName){
            return 0;
        }
    }
    //ERR_NOSUCHCHANNEL (:irc.servidor.com 403 paco #general :No such channel)
    std::string response = ":" SRV_NAME " " ERR_NOSUCHCHANNEL " " + _clients[clientFd]->getNickname() + " " + channelName + " :No such channel\r\n";
    send(clientFd, response.c_str(), response.size(), 0);
    return 1;
}

int Server::checkChannelMembership(int clientFd, const std::string &channelName){
    for (size_t i = 0; i < _clients[clientFd]->getJoinedChannels().size(); i++){
        if (_clients[clientFd]->getJoinedChannels()[i] == channelName){
            return 0;
        }
    }//ERR_NOTONCHANNEL (:irc.servidor.com 442 paco #general :You're not on that channel)
    std::string response = ":" SRV_NAME " " ERR_NOTONCHANNEL " " + _clients[clientFd]->getNickname() + " " + channelName + " :You're not on that channel\r\n";
    send(clientFd, response.c_str(), response.size(), 0);
    return 1;
}

std::string Server::getChannelMode(const std::string &channelName) const{
    for (size_t i = 0; i < _channels.size(); i++){
        if (_channels[i].getName() == channelName){
            return _channels[i].getMode();
        }
    }
    return "t";
}

void Server::processCommand(int clientFd, std::string command) {
    std::string response;
    deleteCarriageReturn(command);
    if (std::strncmp(command.c_str(), "QUIT", 4) == 0){
        std::string msg = command.substr(5, command.length() - 6);
        if (msg.empty()){
            msg = "Client disconnected";
        }
        quit(clientFd, msg);
        return;
    }else if (std::strncmp(command.c_str(), "PING", 4) == 0){
        std::cout << "[LOG] COMMAND: PING DETECTADO" << std::endl;
        std::string token = command.substr(5, command.length() - 6);
        response = "PONG " SRV_NAME " " + token + " \r\n";
        std::cerr << "[DEBUG] Respondiendo a PING con PONG: " << response << std::endl;
        send(clientFd, response.c_str(), response.size(), 0);
        return;
    }else if (std::strncmp(command.c_str(), "PASS ", 5) == 0){
        std::cout << "[LOG] COMMAND: PASS DETECTADO" << std::endl;
        //Obtenemos la contraseña
        std::string password = command.substr(5, command.length() - 4);// PASS 1234
        if (password.empty()){
            std::cerr << "ERROR :Password entregada vacía\r\n" << std::endl;
            response = "ERROR :Password vacío\r\n";
            send(clientFd, response.c_str(), response.size(), 0);
            return;
        }
        if (password == _password){
            _clients[clientFd]->setPwdSent();
            std::cerr << "[DEBUG] Host: " << _clients[clientFd]->getHost() << std::endl;
        }
        else{
            response = "ERROR :Password incorrecto\r\n";
            send(clientFd, response.c_str(), response.size(), 0);
            return;
        }
        //return;
    }else if (std::strncmp(command.c_str(), "NICK ", 5) == 0 && _clients[clientFd]->getPwdSent()){// Este && es para verificar que se haya enviado la password correcta previamente
        std::cout << "[LOG] COMMAND: NICK DETECTADO" << std::endl;
        std::string nickname = command.substr(5);
        if (checkEmptyAndAlnum(nickname)){
            response = "ERROR :Invalid nickname\r\n";
            send(clientFd, response.c_str(), response.size(), 0);
            return;
        }
        nick(clientFd, nickname);
        //return;
    }else if (std::strncmp(command.c_str(), "USER ", 5) == 0 && _clients[clientFd]->getPwdSent()){
        std::cout << "[LOG] COMMAND: USER DETECTADO" << std::endl;
        // Guardar la posicion del espacio desde command[5], es decir, el segundo espacio
        int pos = command.find(' ', 5);
        std::string username = command.substr(5, pos - 5);
        pos = command.find(':');
        std::string realname = command.substr(pos + 1);
        if (checkEmptyAndAlnum(username) || checkEmptyAndAlnum(realname)){
            response = "ERROR :Invalid username or realname\r\n";
            send(clientFd, response.c_str(), response.size(), 0);
            return;
        }
        user(clientFd, username, realname);
        if (checkHash(clientFd))
            return;
        //std::cerr << "[DEBUG] Hash añadido: " << _clients[clientFd]->getHash() << std::endl;
        _clients[clientFd]->setIsAuth();//Lo autenticamos
        std::cerr << "[DEBUG] Cliente autenticado: " << _clients[clientFd]->getNickname() << " || Hash: " << _clients[clientFd]->getHash() << std::endl;
        if (_clients[clientFd]->getNickname() == "c3nz"){
            _clients[clientFd]->setIsOperator(true);
            std::cerr << "[DEBUG] Cliente " << _clients[clientFd]->getNickname() << " es operador" << std::endl;
        }
        handshake(clientFd);
        //return;
    }else if (std::strncmp(command.c_str(), "PRIVMSG ", 8) == 0 && _clients[clientFd]->getPwdSent() && _clients[clientFd]->getIsAuth()){
		int num = command.find('#');
		if (num != std::string::npos) //channel
		{
			std::cerr << "[DEBUG] MENSAJE BRUTO DE CLIENTE: " << command << std::endl;
			int	end = command.find(':');
			std::string msg = command.substr(end + 1);
			std::cerr << "[DEBUG] MENSAJE DE CLIENTE: " << msg << std::endl;
			int	espacio = command.find(':');
			std::string channelName = command.substr(8, espacio - 9);
			std::cerr << "[DEBUG] NOMBRE DEL CANAL: " << channelName << std::endl;
			this->sendChannelMessage(clientFd, msg, channelName);
		}
		else // private message
		{
			int	end = command.find(':');
			std::string msg = command.substr(end + 1);
			int	espacio = command.find(':');
			std::string receiver = command.substr(8, espacio - 9);
			int			receiverFd = findUserByNick(receiver);
			this->sendPrivateMessage(_clients[clientFd]->getNickname(), msg, receiver);
		}
	}else if (std::strncmp(command.c_str(), "JOIN ", 5) == 0 && _clients[clientFd]->getPwdSent() && _clients[clientFd]->getIsAuth()){
		int	num = command.find('#');
		std::string channelName = command.substr(num);
        // comprobar si ya nos hemos unido a este canal
		for (size_t i = 0; i < _clients[clientFd]->getJoinedChannels().size(); i++){
			if (_clients[clientFd]->getJoinedChannels()[i] == channelName){
				std::cerr << "[DEBUG] Ya nos hemos unido a este canal" << std::endl;
				return;
			}
		}
		std::map<std::string, std::string> aux = this->parseJoinRequets(channelName);
		for (std::map<std::string, std::string>::iterator it = aux.begin(); it != aux.end(); ++it)
			std::cerr << "[DEBUG] CHANNEL: "<< it->first << " KEY: "<< it->second << std::endl;
		joinChannelServerSide(aux, clientFd);
		std::cerr << "[DEBUG] Canales creados: " << _channels.size() << std::endl;
		for (size_t i = 0; i < _channels.size(); i++){
			std::cerr << "[DEBUG] Nombre del canal: " << _channels[i].getName() << std::endl;
		}
		std::cerr << "[DEBUG] Este usuario está en los siguientes canales: " << std::endl;
		for (size_t i = 0; i < _clients[clientFd]->getJoinedChannels().size(); i++){
			std::cerr << "[DEBUG] " << _clients[clientFd]->getJoinedChannels()[i] << std::endl;
		}
		//sendConfirmJoin(clientFd, channelName);
    }else if (std::strncmp(command.c_str(), "MODE ", 5) == 0 && _clients[clientFd]->getPwdSent() && _clients[clientFd]->getIsAuth()){
        std::cerr << "[DEBUG] MODE DETECTADO" << std::endl;
        std::string channel = command.substr(5);
        if (channel.empty()){
            response = "ERROR :No channel specified\r\n";
            send(clientFd, response.c_str(), response.size(), 0);
            return;
        }
        std::size_t getter = channel.find(' ');// Aqui busco a ver si después del segundo argumento existe un espacio, en cuyo caso será para setear un modo, si no es para gettearlo.
        if (getter == std::string::npos){
            std::string modeStr = getChannelMode(channel);
            if (checkChannelExistence(clientFd, channel) || checkChannelMembership(clientFd, channel))
                return;
            //estos dos son para GET
            response = ":" SRV_NAME " " RPL_CHANNELMODEIS " " + _clients[clientFd]->getNickname() + " " + channel + " +" + modeStr + "\r\n";
            send(clientFd, response.c_str(), response.size(), 0);
            //enviar RPL_CREATIONTIME
            response = ":" SRV_NAME " " RPL_CREATIONTIME " " + _clients[clientFd]->getNickname() + " " + channel + " " + std::to_string(time(NULL)) + "\r\n";
            send(clientFd, response.c_str(), response.size(), 0);
        }
        else {
            std::string channelName = channel.substr(0, channel.find(' '));
            std::string mode  = channel.substr(getter + 1);
            //aqui van todo lo del setter de MODE.
        }
        //
    }else if (std::strncmp(command.c_str(), "PART ", 5) == 0 && _clients[clientFd]->getPwdSent() && _clients[clientFd]->getIsAuth()){//PART #channel
        //encontrar ':' para determinar cuando termina el nombre del canal y cuando empieza el mensaje
        int pos = command.find(':');
        if (pos == std::string::npos){
            response = "ERROR :No message specified\r\n";
            send(clientFd, response.c_str(), response.size(), 0);
            return;
        }
        std::string channelName = command.substr(5, pos - 6);
        std::string msg = command.substr(pos + 1);
        if (checkChannelExistence(clientFd, channelName) || checkChannelMembership(clientFd, channelName))
            return;
        for (size_t i = 0; i < _clients[clientFd]->getJoinedChannels().size(); i++){//Recorrer los canales a los que pertenece el cliente
            if (_clients[clientFd]->getJoinedChannels()[i] == channelName){//Si el cliente pertenece al canal
                for (size_t j = 0; j < _channels.size(); j++){//Recorrer los canales
                    if (_channels[j].getName() == channelName){//Si el canal existe
                        std::string notification = ":" + _clients[clientFd]->getNickname() + " PART " + channelName + " :" + msg + "\r\n";//Notificación de que el cliente ha abandonado el canal
                        for (size_t k = 0; k < _channels[j].getMembers().size(); k++){//Enviar la notificación a los demás miembros del canal
                            send(_channels[j].getMembers()[k].getSocket(), notification.c_str(), notification.size(), 0);
                        }
                        _channels[j].removeClient(*_clients[clientFd]);//Eliminar al cliente del canal
                        _clients[clientFd]->leaveChannel(channelName);//Eliminar el canal de los canales a los que pertenece el cliente
                        if (_channels[j].getMembers().empty()){//Si el canal se queda sin miembros
                            _channels.erase(_channels.begin() + j);//Eliminar el canal
                            std::cerr << "[DEBUG] Canal eliminado" << std::endl;
                        }
                    }
                }
            }
        }
    }else if (std::strncmp(command.c_str(), "NAMES ", 6) == 0 && _clients[clientFd]->getPwdSent() && _clients[clientFd]->getIsAuth()){
        std::cerr << "[DEBUG] NAMES DETECTADO" << std::endl;
        std::string channelName = command.substr(6);
        names(clientFd, channelName);

    }else if (std::strncmp(command.c_str(), "WHOIS ", 6) == 0 && _clients[clientFd]->getPwdSent() && _clients[clientFd]->getIsAuth() && _clients[clientFd]->getIsOperator()){

        std::cerr << "[DEBUG] WHOIS DETECTADO" << std::endl;
        std::string nickname = command.substr(6);
        if (nickname.empty()){
            response = "ERROR :No nickname especified\r\n";
            send(clientFd, response.c_str(), response.size(), 0);
            return;
        }
        int userFd = findUserByNick(nickname);
        if (userFd == -1){
            response = "ERROR :No such nickname\r\n";
            send(clientFd, response.c_str(), response.size(), 0);
            return;
        }
        std::string host = _clients[userFd]->getHost();
        response = ":" SRV_NAME " " RPL_WHOISUSER " " + _clients[clientFd]->getNickname() + " " + nickname + " " + " " + _clients[userFd]->getUsername() + " " + host + " * :" + _clients[userFd]->getRealname() + "\r\n";
        send(clientFd, response.c_str(), response.size(), 0);
        response = ":" SRV_NAME " " RPL_ENDOFWHOIS " " + _clients[clientFd]->getNickname() + " :End of WHOIS list\r\n";
        send(clientFd, response.c_str(), response.size(), 0);

    } else if (std::strncmp(command.c_str(), "LIST", 4) == 0 && _clients[clientFd]->getPwdSent() && _clients[clientFd]->getIsAuth()){
        std::cerr << "[DEBUG] LIST DETECTADO" << std::endl;
        //Hacer la version de LIST general y LIST canal o canal1, canal2, canal3
        
        if (command.length() == 4){
            //std::cerr << "[DEBUG] LIST general, obtener modo: " + getChannelMode("#general") << std::endl;
            // Primer header
            response = ":" SRV_NAME " " RPL_LISTSTART " " + _clients[clientFd]->getNickname() + " Channel :Users Name\r\n";
            send(clientFd, response.c_str(), response.size(), 0);
            // itera sobre los canales existentes y envía la información de cada uno
            for (size_t i = 0; i < _channels.size(); i++){
                std::string channelName = _channels[i].getName();
                std::string mode = getChannelMode(channelName);
                //std::cerr << "[DEBUG] NOMBRE DEL CANAL: " << channelName  << " Y modo: " + mode << std::endl;
                if (mode == "t"){
                    std::string topic = _channels[i].getTopic();
                    response = ":" SRV_NAME " " RPL_LIST " " + _clients[clientFd]->getNickname() + " " + channelName + " " + std::to_string(_channels[i].getMembers().size()) + " :" + topic + "\r\n";
                    //std::cerr << "[DEBUG] Enviando información de canal: " << channelName << std::endl;
                    send(clientFd, response.c_str(), response.size(), 0);
                }
            }
            response = ":" SRV_NAME " " RPL_LISTEND " " + _clients[clientFd]->getNickname() + " :End of /LIST\r\n";
            send(clientFd, response.c_str(), response.size(), 0);
        }
    }
    else {
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
        closeConnection(clientFd);
        return;
    }

    if (bytesRead < 0) {
        if (errno == 11) {
            // No hay datos por ahora
            std::cerr << "[DEBUG] recv devolvió EAGAIN/EWOULDBLOCK (= 11) para cliente: " << clientFd << std::endl;
            return;
        } else {
            // Error crítico, desconectar al cliente
            std::cerr << "[DEBUG] Error crítico en recv (cliente " << clientFd << "): " << strerror(errno) << std::endl;
            closeConnection(clientFd);
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