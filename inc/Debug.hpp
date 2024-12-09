#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>
#include <string>

#define DBGLVL 1

template <typename T>
class Debug {
    public:
        static void debug(const std::string &message, const T &value) {
            if (DBGLVL == 1) {
                std::cerr << "[DEBUG] " << message << ": " << value << std::endl;
            }
        }//Example of use: Debug<std::string>::debug("Error al acumular datos en el buffer", e.what());

};


#endif