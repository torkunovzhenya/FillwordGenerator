#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <windows.h>
#include <wchar.h>
#include <sys/stat.h>
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
    P_DictionaryAddAnsRequest,
    P_DictAlreadyExist,
    P_DictNotExist
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

bool ReadDict(int index, const string& dict_name)
{
    int len;

    // Receiving dictionary lenght
    if (!ReadInt(index, len))
        return false;

    fstream dict;
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
    return true;
}


bool Send(int index, Packet packet)
{
    return send(Connections[index], (char*)&packet, sizeof(int), 0) == sizeof(int);
}

bool Send(int index, int n)
{
    return send(Connections[index], (char*)&n, sizeof(int), 0) == sizeof(int);
}

bool Send(int index, string& msg)
{
    Send(index, msg.size());
    if (msg.empty())
        return true;
    return send(Connections[index], msg.c_str(), msg.size(), 0);
}

bool Send(int index, const vector<int>& vector)
{
    int size = vector.size();
    Send(index, size);

    for (int i = 0; i < size; ++i)
        Send(index, vector[i]);

    return true;
}


void CancelHandler(int index)
{
    Packet packet;
    while (generating[index])
    {
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

            // Prepare dictionary for work
            DictionaryWorker::createDictionaryWords(dict);

            // Initialize new algorithm
            algos[index] = new DancingLinks(h, w, min_l, max_l, dict);

            // Create new thread to get Cancel Request from user
            generating[index] = true;
            HANDLE t = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE) CancelHandler,
                                    (LPVOID) index, 0, nullptr);

            string msg;
            vector<int> colors;

            // Start generation
            if (algos[index]->FindSolution() && !algos[index]->Stopped())
                msg = algos[index]->getRes(colors); // If generation is ended and not stopped - gets generation data
            else
                msg = "Error";

            // If field algorithm ended his work before Cancel Request from user - kill cancel thread
            TerminateThread(t, 0);

            // Checking if all words are english
            bool english = (msg.back() >= 'a' && msg.back() <= 'z');
            if (!english)
                ChangeString(msg);  // If words are russian - change their codes;

            Send(index, P_FieldAnsRequest);
            Send(index, english);
            Send(index, msg);

            if (msg != "Error")
                Send(index, colors);

            delete algos[index];
            return true;
        }
        case P_DictionaryAddRequest:
        {
            // Receiving new dictionary name
            string dict_name;
            if (!ReadString(index, dict_name))
                return false;

            // Checking if dictionary with this name already exists
            if (DictionaryWorker::alreadyExist(dict_name))
            {
                Send(index, P_DictAlreadyExist);
                return true;
            }
            else
                Send(index, P_DictNotExist);

            // Create new directory for new dictionary
            CreateDirectory(("../Dictionaries/" + dict_name).c_str(), nullptr);

            ReadDict(index, dict_name);
            bool good = DictionaryWorker::checkDictionary(dict_name);

            // If file has incorrect format - delete it and directory
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
            // getting all directories in dictionaries
            vector<string> dicts = DictionaryWorker::getDictionaries();
            string msg;
            for (const string& dict : dicts)
                msg += dict + "\n";

            Send(index, msg);

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


bool existDir(const char * name)
{
    struct stat s;
    if (stat(name, &s)) return false;
    return S_ISDIR(s.st_mode);
};



int main(int argc, char* argv[])
{
    if (!existDir("../Dictionaries/"))
        CreateDirectory("../Dictionaries/", nullptr);

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

    cout << "Server started" << endl;

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