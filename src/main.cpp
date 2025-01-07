#include "Server.hpp"

void checkPort(std::string port)
{
	for (size_t i = 0; i < port.length(); i++)
	{
		if (!isdigit(port[i]))
		{
			std::cerr << "Invalid port" << std::endl;
			exit(1);
		}
	}
	if (std::stoi(port) < 1 || std::stoi(port) > 49151)
	{
		std::cerr << "Invalid port" << std::endl;
		exit(1);
	}
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: ./server <port>" << std::endl;
		return 1;
	}
	std::string pwd = argv[2];
	if (pwd.empty())
	{
		std::cerr << "Password cannot be empty" << std::endl;
		return 1;
	}

	for (size_t i = 0; i < pwd.length(); i++)
	{
		if (!isdigit(pwd[i]))
		{
			std::cerr << "Invalid password" << std::endl;
			exit(1);
		}
	}
	checkPort(argv[1]);
	Server server(std::stoi(argv[1]), pwd, ":Welcome to the Anon Chat Server");
	server.start();
	server.run();
	return 0;
}