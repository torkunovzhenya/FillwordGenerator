#ifndef FILLWORDSERVER_SERVERMANAGER_HPP
#define FILLWORDSERVER_SERVERMANAGER_HPP

#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>

class ServerManager
{
public:

    void StartServer();

    ~ServerManager();

private:

    const char *adress = "127.0.0.1";
    int port = 1111;
    SOCKADDR_IN addr;
    SOCKET sListen;
};

#endif //FILLWORDSERVER_SERVERMANAGER_HPP
