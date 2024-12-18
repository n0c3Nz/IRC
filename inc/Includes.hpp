#ifndef INCLUDES_HPP
#define INCLUDE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <list>
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
#include <poll.h>
#include <sys/epoll.h>
#include <memory>
#include <exception>

#include "ErrorHandler.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "Defines.hpp"

#endif