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
static bool generating[MAX_CONNECTIONS];
static DancingLinks* algos[MAX_CONNECTIONS];


enum Packet {
    P_ConnectionRequest = 1,
    P_FieldGenRequest,
    P_StopGenRequest,
    P_FieldAnsRequest,
    P_DictionariesListRequest,
    P_DictionaryAddRequest,
    P_DictionaryAddAnsRequest
};

bool ReadString(int index, string& s)
{
    int msg_size;

    if (recv(Connections[index], (char*)&msg_size, sizeof(int), 0) <= 0)
        return false;

    char* msg = new char[msg_size + 1];
    msg[msg_size] = '\0';

    if (recv(Connections[index], msg, msg_size, 0) <= 0)
        return false;

    s = msg;

    return true;
}

bool ReadInt(int index, int& n)
{
    return recv(Connections[index], (char*)&n, sizeof(int), 0) > 0;
}


bool Send(int index, Packet packet)
{
    return send(Connections[index], (char*)&packet, sizeof(int), 0) == sizeof(int);
}


bool Send(int index, int n)
{
    return send(Connections[index], (char*)&n, sizeof(int), 0) == sizeof(int);
}


void CancelThread(int index)
{
    while (generating[index])
    {
        Packet packet;
        recv(Connections[index], (char *) &packet, sizeof(Packet), 0);

        if (packet == P_StopGenRequest)
        {
            cout << "Cancelled!" << endl;
            algos[index]->Stop();
            generating[index] = false;
        }
    }
}


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
        case P_ConnectionRequest:
        {
            Send(index, P_ConnectionRequest);
            return true;
        }
        case P_FieldGenRequest:
        {
            int h, w;
            int min_l, max_l;

            if (!ReadInt(index, h))
                return false;
            if (!ReadInt(index, w))
                return false;
            if (!ReadInt(index, min_l))
                return false;
            if (!ReadInt(index, max_l))
                return false;

            string dict;
            if (!ReadString(index, dict))
                return false;

            cout << "Generation parameters for " << index << " client: " <<
                    h << " " << w << " " << min_l << " " << max_l << " " << dict << endl;

            createDictionaryWords(dict);

            algos[index] = new DancingLinks(h, w, min_l, max_l, dict);

            string msg;
            vector<int> colors;

            generating[index] = true;
            HANDLE t = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)CancelThread,
                         (LPVOID)index, 0, nullptr);

            if (algos[index]->FindSolution() && !algos[index]->Stopped())
                msg = algos[index]->getRes(colors);
            else
                msg = "Error";

            TerminateThread(t, 0);

            int msg_size = msg.size();
            int col_size = colors.size();
            bool english = (msg[msg_size - 1] >= 'a' && msg[msg_size - 1] <= 'z');

            if (!english)
                ChangeString(msg);

            Send(index, P_FieldAnsRequest);
            Send(index, english);
            send(Connections[index], (char*)&msg_size, sizeof(int), 0);
            send(Connections[index], msg.c_str(), msg_size, 0);

            if (msg != "Error")
            {
                send(Connections[index], (char*)&col_size, sizeof(int), 0);
                for (int i = 0; i < col_size; ++i)
                    send(Connections[index], (char*)&colors[i], sizeof(int), 0);
            }

            delete algos[index];
            return true;
        }
        case P_DictionaryAddRequest:
        {
            int len;
            string dict_name;

            // Receiving new dictionary name
            if (!ReadString(index, dict_name))
                return false;

            // Receiving dictionary lenght
            if (!ReadInt(index, len))
                return false;

            fstream dict;

            CreateDirectory(("../Dictionaries/" + dict_name).c_str(), nullptr);
            dict.open("../Dictionaries/" + dict_name + "/Dictionary.txt", ios::out);

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

            bool good = checkDictionary(dict_name);

            if (!good)
            {
                DeleteFile(("../Dictionaries/" + dict_name + "/Dictionary.txt").c_str());
                RemoveDirectory(("../Dictionaries/" + dict_name).c_str());
            }
            Send(index, P_DictionaryAddAnsRequest);
            Send(index, good);

            return true;
        }
        case P_DictionariesListRequest:
        {
            vector<string> dicts = getDictionaries();
            string msg;
            for (const string& dict : dicts)
                msg += dict + "\n";

            int msg_size = msg.size();

            send(Connections[index], (char*)&msg_size, sizeof(int), 0);
            send(Connections[index], msg.c_str(), msg_size, 0);

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

        if (packettype != P_ConnectionRequest)
            cout << "Packet number " << (int)packettype << endl;

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
        cout << "WSA Error" << endl;
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