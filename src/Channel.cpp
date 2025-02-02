#include <Channel.hpp>

Channel::Channel() {
	this->_name = "Default";
	this->_isPrivate = false;
	this->_topic = "No topic is set";
	this->_mode = "t";
	this->_Pwd = false;
	this->maxMembers = -1;
	this->_createdTime = std::to_string(time(NULL));
}

Channel::Channel(std::string name) {
	this->_name = name;
	this->_isPrivate = false;
	this->_topic = "No topic is set";
	this->_mode = "t";
	this->_Pwd = false;
	this->maxMembers = -1;
	this->_createdTime = std::to_string(time(NULL));
}

Channel::Channel(const Channel &copy) {
	this->_name = copy._name;
	this->_topic = copy._topic;
	this->_members = copy._members;
	this->_operators = copy._operators;
	this->_password = copy._password;
	this->_isPrivate = copy._isPrivate;
	this->_Pwd = copy._Pwd;
	this->_mode = copy._mode;
	this->maxMembers = copy.maxMembers;
	this->_createdTime = copy._createdTime;
}

Channel &Channel::operator=(const Channel &copy) {
	if (this != &copy) {
		this->_name = copy._name;
		this->_isPrivate = copy._isPrivate;
		this->_topic = copy._topic;
		this->_members = copy._members;
		this->_operators = copy._operators;
		this->_password = copy._password;
		this->_Pwd = copy._Pwd;
		this->_mode = copy._mode;
		this->maxMembers = copy.maxMembers;
		this->_createdTime = copy._createdTime;
	}
	return *this;
}

Channel::~Channel() {
}

std::string		Channel::getName() const
{
	return this->_name;
}

std::vector<Client>	Channel::getMembers() const
{
	return this->_members;
}

std::string	Channel::getMode() const
{
	return this->_mode;
}

std::string Channel::getTopic() const
{
	return this->_topic;
}

void	Channel::setMode(const std::string &mode)
{
	this->_mode = mode;
}

void	Channel::setTopic(const std::string &topic)
{
	this->_topic = topic;
}

void	Channel::addOperator(const std::string &nick)
{
	//comprobar si es miembro
	if (alreadyIn(nick) == false)
		return;
	//comprobar si ya es operador
	for (size_t i = 0; i < this->_operators.size(); i++)
	{
		if (this->_operators[i] == nick)
			return;
	}
	this->_operators.push_back(nick);
}

void	Channel::removeOperator(const std::string &nick)
{
	//comprobar si es miembro
	if (alreadyIn(nick) == false)
		return;
	for (size_t i = 0; i < this->_operators.size(); i++)
	{
		if (this->_operators[i] == nick)
		{
			std::cerr << "[DEBUG] Efectivamente el cliente era operador" << std::endl;
			//eliminar el operador[i]
			this->_operators.erase(this->_operators.begin() + i);
		}
		std::string answer = isOperator(nick) ? "YES" : "NO";
		std::cerr << "[DEBUG] ¿Ahora es operador? " << answer << std::endl;
	}
}

bool	Channel::alreadyIn(const std::string &nickName)
{
	for (size_t i = 0; i < this->_members.size(); i++)
		if (this->_members[i].getNickname() == nickName)
			return true;
	return false;
}

void		Channel::addClient(Client &client)
{
 	//Si no hay nadie en el canal, el primer cliente que se une es operador
	if (this->_members.size() == 0)
 		this->_operators.push_back(client.getNickname());
	if (alreadyIn(client.getNickname()) == false)
 		_members.push_back(client);
 	std::cerr << "[DEBUG] Cliente añadido: " << client.getNickname() << " al canal: " << this->_name << std::endl;
}

//Devuelve 1 si es operador, 0 en caso contrario
int Channel::isOperator(const std::string &nick)
{
	for (size_t i = 0; i < this->_operators.size(); i++)
	{
		if (this->_operators[i] == nick)
			return 1;
	}
	return 0;
}

void Channel::removeClient(Client &client)
{
	for (size_t i = 0; i < this->_members.size(); i++)
	{
		if (this->_members[i].getNickname() == client.getNickname())
		{
			this->_members.erase(this->_members.begin() + i);
			std::cerr << "[DEBUG] Cliente eliminado: " << client.getNickname() << " del canal: " << this->_name << std::endl;
			return;
		}
	}
}

int		Channel::findUserFd(const std::string &nick) const
{
	for (size_t i = 0; i < this->_members.size(); i++)
		if (this->_members[i].getNickname() == nick)
			return this->_members[i].getSocket();
	return 0;
}

std::string	Channel::getUsernameByNick(const std::string &nickName) const
{
	for (size_t i = 0; i < this->_members.size(); i++)
		if (this->_members[i].getNickname() == nickName)
			return this->_members[i].getUsername();
	return 0;
}

void		Channel::sendMsg(const std::string &senderNick, const std::string &host, const std::string &msg)
{
	int	senderFd = findUserFd(senderNick);
	std::string senderUsername = getUsernameByNick(senderNick);
	std::string fullMsg = ":" + senderNick + "!" + senderUsername + "@" + host + " PRIVMSG " + this->getName() + " :" + msg + "\r\n";
	std::cerr << "[DEBUG] FULL MSG: " << fullMsg << std::endl;
	for (size_t i = 0; i < this->_members.size(); i++)
	{
		int receiverFd = findUserFd(this->_members[i].getNickname());
		std::cerr << "[DEBUG] RECEIVERS: " << this->_members[i].getNickname() << std::endl;
		if (receiverFd != senderFd && receiverFd != -1)
		{
			send(receiverFd, fullMsg.c_str(), fullMsg.size(), 0);
		}
	}
}

bool	Channel::getIfPwd() const
{
	return this->_Pwd;
}

std::string Channel::getPwd() const
{
	return this->_password;
}

void	Channel::setIfPwd(const bool &pwd)
{
	this->_Pwd = true;
}

void	Channel::setisPrivate(bool isPrivate)
{
	this->_isPrivate = isPrivate;
}

void	Channel::setPwd(const std::string &pwd)
{
	this->_password = pwd;
}

void	Channel::clearInvitedList()
{
	this->_invited.clear();
}

void	Channel::addInvited(const std::string &nick)
{
	for (size_t i = 0; i < this->_invited.size(); i++)
	{
		if (this->_invited[i] == nick)
			return;
	}
	this->_invited.push_back(nick);
}

void	Channel::addMembersToInvitedList()
{
	for (size_t i = 0; i < this->_members.size(); i++)
	{
		this->_invited.push_back(this->_members[i].getNickname());
	}
}

bool	Channel::isInvited(const std::string &nick)
{
	for (size_t i = 0; i < this->_invited.size(); i++)
	{
		if (this->_invited[i] == nick)
			return true;
	}
	return false;
}

bool Channel::getIsPrivate() const
{
	return this->_isPrivate;
}


// Devuelve el número máximo de miembros que puede tener el canal
int Channel::getMaxMembers() const
{
	return this->maxMembers;
}
// Establece el número máximo de miembros que puede tener el canal:
// -1: Sin límite
void Channel::setMaxMembers(int maxMembers)
{
	this->maxMembers = maxMembers;
}

bool Channel::isMember(const std::string &nick)
{
	for (size_t i = 0; i < this->_members.size(); i++)
	{
		if (this->_members[i].getNickname() == nick)
			return true;
	}
	return false;
}

std::string Channel::getCreatedTime() const
{
	return this->_createdTime;
}