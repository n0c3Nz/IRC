#ifndef DEFINES_HPP
# define DEFINES_HPP

// General
#define MAX_EVENTS 10
#define MAX_MSG_SIZE 4096

// Server Handshake
#define SRV_NAME "AnonChatServer"
#define SRV_VERSION "0.1"
#define RPL_WELCOME "001"
#define RPL_YOURHOST "002"
#define RPL_CREATED "003"
#define RPL_MYINFO "004"

// MOTD
#define RPL_MOTDSTART "375"
#define RPL_MOTD "372"
#define RPL_ENDOFMOTD "376"

// WHOIS
#define RPL_WHOISUSER "311"
#define RPL_ENDOFWHOIS "318"

// JOIN
#define RPL_TOPIC "332"
#define RPL_NAMREPLY "353"
#define RPL_ENDOFNAMES "366"
#define ERR_INVITEONLYCHAN "473"

// JOIN
#define ERR_BADCHANNELKEY "475"
#define ERR_NOTONCHANNEL "442"
#define ERR_NOSUCHCHANNEL "403"

// MODE
#define RPL_CHANNELMODEIS "324"
#define RPL_CREATIONTIME "329"

// NICK
#define ERR_NOSUCHNICK "401"

// LIST
#define RPL_LISTSTART "321"
#define RPL_LIST "322"
#define RPL_LISTEND "323"

// TOPIC
#define ERR_CHANOPRIVSNEEDED "482"//(482) si no tiene permisos.
//#define ERR_NOTONCHANNEL "442"//(442) si no está en el canal.
#define ERR_NEEDMOREPARAMS "461"//(461) si faltan parámetros.

//MODES
#define MODES "kiot"

//INVITE
#define RPL_INVITING "341"

#endif