#include "ServerManager.hpp"
#include "DancingLinks.hpp"

using namespace std;

SOCKET Connections[100];
int connections_counter = 0;


void ClientHandler(int index)
{
    int msg_size;
    while (true)
    {
        recv(Connections[index], (char*)&msg_size, sizeof(int), 0);
        char* msg = new char[msg_size + 1];
        msg[msg_size] = '\0';

        int bytes = recv(Connections[index], msg, msg_size, 0);

        if (bytes == SOCKET_ERROR)
        {
            cout << to_string(index) << " disconnected" << endl;
            closesocket(Connections[index]);
            return;
        }

        for (int i = 0; i < connections_counter; ++i)
        {
            if (i == index)
                continue;

            send(Connections[i], (char*)&msg_size, sizeof(int), 0);
            send(Connections[i], msg, msg_size, 0);
        }

        delete[] msg;
    }
}


void ServerManager::StartServer()
{
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData) != 0)
    {
        cout << "Error" << endl;
        exit(1);
    }

    int sizeofaddr = sizeof(addr);
    addr.sin_addr.s_addr = inet_addr(adress);
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;

    sListen = socket(AF_INET, SOCK_STREAM, 0);
    bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
    listen(sListen, 10);

    SOCKET newConnetion;
    for (int i = 0; i < 100; ++i)
    {
        newConnetion = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);

        if (newConnetion == 0)
            cout << "Client failed to connect" << endl;
        else
        {
            cout << "Client " << to_string(i) << " connected" << endl;

            string msg = "Connected!";
            int msg_size = msg.size();
            send(newConnetion, (char*)&msg_size, sizeof(int), 0);
            send(newConnetion, msg.c_str(), msg_size, 0);

            Connections[connections_counter] = newConnetion;
            connections_counter++;

            CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)ClientHandler,
                    (LPVOID)i, 0, nullptr);
        }
    }

    cout << "Server ended its work!" << endl;
    system("pause");
}

ServerManager::~ServerManager()
{
    for (int i = 0; i < connections_counter; ++i)
        closesocket(Connections[i]);

    closesocket(sListen);
    delete[] adress;
}
