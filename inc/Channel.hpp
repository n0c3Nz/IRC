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
		std::vector <std::string>		_invited;
		std::string						_password;
		bool							_isPrivate;
		bool							_Pwd;
		std::string						_mode;
		int								maxMembers;
		std::string						_createdTime;
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
		bool 			getIsPrivate() const;
		int 			getMaxMembers() const;
		std::string		getCreatedTime() const;
		void			setMaxMembers(int maxMembers);
		void			setMode(const std::string &mode);
		void			setTopic(const std::string &topic);
		void			setPwd(const std::string &pwd);
		void			setIfPwd(const bool &pwd);
		void 			setisPrivate(bool isPrivate);
		void			addOperator(const std::string &nick);
		void			removeOperator(const std::string &nick);
		void			addInvited(const std::string &nick);
		void			addMembersToInvitedList();
		bool			isInvited(const std::string &nick);
		bool 			isMember(const std::string &nick);
		void 			clearInvitedList();
		void			addClient(Client &client);
		int 			isOperator(const std::string &nick);
		void			removeClient(Client &client);
		void			sendMsg(const std::string &senderNick, const std::string &host,const std::string &msg);
		int 			findUserFd(const std::string &senderNick) const;
		std::string		getUsernameByNick(const std::string &nickName) const;
		bool			alreadyIn(const std::string &nickName);
};


#endif