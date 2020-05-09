#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <windows.h>
#include <wchar.h>
#include "DancingLinks.hpp"
#include "DictionaryWorker.hpp"

using namespace std;


const char* adress = "127.0.0.1";
int port = 1111;
const int MAX_CONNECTIONS = 10;
static SOCKET Connections[MAX_CONNECTIONS];
static int connections_counter = 0;


enum Packet {
    P_ConnectionRequest = 0,
    P_FieldGenRequest,
    P_FieldAnsRequest,
    P_DictionaryAddRequest,
    P_DictionaryAddAnsRequest
};


void ChangeString(string& s)
{
    wchar_t begin = 65504;
    wchar_t end = begin + 31;
    for (int i = 0; i < s.length(); ++i)
    {
        wchar_t c = s[i];
        if (c >= begin && c <= end)
        {
            c -= begin - L'a' + 1;
            s[i] = c;
        }
    }
}


bool ProcessPacket(int index, Packet packettype)
{
    switch(packettype)
    {
        case P_FieldGenRequest:
        {
            int h, w;
            int min_l, max_l;
            string dict = "NewDictionary";

            if (recv(Connections[index], (char*)&h, sizeof(int), 0) <= 0)
                return false;
            if (recv(Connections[index], (char*)&w, sizeof(int), 0) <= 0)
                return false;
            if (recv(Connections[index], (char*)&min_l, sizeof(int), 0) <= 0)
                return false;
            if (recv(Connections[index], (char*)&max_l, sizeof(int), 0) <= 0)
                return false;

            cout << h << " " << w << " " << min_l << " " << max_l << endl;

            cout << (createDictionaryWords(dict) ? "true" : "false") << endl;
            DancingLinks* algo = new DancingLinks(h, w, min_l, max_l, dict);

            string msg;
            vector<int> colors;

            if (algo->FindSolution())
                msg = algo->getRes(colors);
            else
                msg = "Error";

            ChangeString(msg);
            int msg_size = msg.size();
            int col_size = colors.size();
            cout << col_size << endl;

            Packet sendingpacket = P_FieldAnsRequest;
            send(Connections[index], (char*)&sendingpacket, sizeof(int), 0);
            send(Connections[index], (char*)&msg_size, sizeof(int), 0);
            send(Connections[index], msg.c_str(), msg_size, 0);

            if (msg != "Error")
            {
                send(Connections[index], (char*)&col_size, sizeof(int), 0);
                for (int i = 0; i < col_size; ++i)
                    send(Connections[index], (char*)&colors[i], sizeof(int), 0);
            }

            delete algo;
            return true;
        }
        case P_DictionaryAddRequest:
        {
            int len;

            if (recv(Connections[index], (char*)&len, sizeof(int), 0) <= 0)
                return false;

            string name = "NewDictionary";

            fstream dict;

            CreateDirectory(("../Dictionaries/" + name).c_str(), NULL);
            dict.open("../Dictionaries/" + name + "/Dictionary.txt", ios::out);

            if (!dict.is_open())
                return false;

            for (int i = 0; i < len; ++i)
            {
                byte byte;
                recv(Connections[index], (char*)&byte, sizeof(byte), 0);
                if (byte != 13)
                    dict.put(byte);
            }

            dict.close();

            Packet sendingpacket = P_DictionaryAddAnsRequest;
            send(Connections[index], (char*)&sendingpacket, sizeof(int), 0);

            return true;
        }
        default:
        {
            cout << "Unrecognized packet" << endl;
            return false;
        }
    }
}


void ClientHandler(int index)
{
    Packet packettype;
    while (true)
    {
        int bytes = recv(Connections[index], (char *) &packettype, sizeof(Packet), 0);
        cout << (int)packettype << endl;

        if (bytes == SOCKET_ERROR || bytes == 0 || !ProcessPacket(index, packettype))
            break;
    }

    cout << to_string(index) << " disconnected" << endl;
    closesocket(Connections[index]);

    Connections[index] = 0;
    connections_counter--;
}


int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "Russian");
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData) != 0)
    {
        cout << "Error" << endl;
        exit(1);
    }

    SOCKADDR_IN addr;
    int sizeofaddr = sizeof(addr);
    addr.sin_addr.s_addr = inet_addr(adress);
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;

    SOCKET sListen;
    sListen = socket(AF_INET, SOCK_STREAM, 0);
    bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
    listen(sListen, 10);

    SOCKET newConnetion;
    for (int i = 0; i < 100; ++i)
    {
        newConnetion = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);

        if (connections_counter == MAX_CONNECTIONS)
        {
            cout << "Can't take new client" << endl;
            closesocket(newConnetion);
            continue;
        }

        if (newConnetion == 0)
            cout << "Client failed to connect" << endl;
        else
        {
            connections_counter++;

            string msg = "Connected!";
            int msg_size = msg.size();
            send(newConnetion, (char*)&msg_size, sizeof(int), 0);
            send(newConnetion, msg.c_str(), msg_size, 0);

            int index = 0;
            for (; index < MAX_CONNECTIONS; ++index)
            {
                cout << Connections[index] << endl;
                if (Connections[index] == 0)
                {
                    Connections[index] = newConnetion;
                    break;
                }
            }

            cout << "Client " << to_string(index) << " connected" << endl;
            CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)ClientHandler,
                         (LPVOID)index, 0, nullptr);
        }
    }

    cout << "Server ended its work!" << endl;
    system("pause");

    return 0;
}