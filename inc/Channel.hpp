#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Includes.hpp"

class Client;

class Channel {
	private:
		std::string						_name;
		std::string						_topic;
		std::vector<Client>				_members;
		std::vector<std::string>		_operators;
		std::string						_password;
		bool							_isPrivate;
		bool							_Pwd;
		std::string						_mode;
	public:
		Channel();
		Channel(std::string name);
		Channel(const Channel &copy);
		Channel &operator=(const Channel &copy);
		~Channel();
		std::string		getName() const;
		std::vector<Client>	getMembers() const;
		bool			getIfPwd() const;
		std::string		getPwd() const;
		std::string		getMode() const;
		std::string		getTopic() const;
		void			addOperator(const std::string &nick);
		void			removeOperator(const std::string &nick);
		void			addClient(Client &client);
		int 			isOperator(const std::string &nick);
		void			removeClient(Client &client);
		void			sendMsg(const std::string &senderNick, const std::string &host,const std::string &msg);
		int 			findUserFd(const std::string &senderNick) const;
		std::string		getUsernameByNick(const std::string &nickName) const;
		bool			alreadyIn(const std::string &nickName);
};


#endif