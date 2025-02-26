﻿using System;
using System.Net.Sockets;
using System.Text;
using System.IO;

namespace FillwordGameLibrary
{
    enum Packet
    {
        P_ConnectionRequest = 1,
        P_FieldGenRequest,
        P_StopGenRequest,
        P_FieldAnsRequest,
        P_DictionariesListRequest,
        P_DictionaryAddRequest,
        P_DictionaryAddAnsRequest,
        P_DictAlreadyExist,
        P_DictNotExist,
        P_Error
    };


    public static class Manager
    {
        private const string address = "127.0.0.1";
        private const int port = 1111;

        private static TcpClient client = null;
        private static NetworkStream stream = null;
        private static bool connected = false;
        private static bool busy = false;

        public static bool Connected { get => connected; }
        public static bool isBusy { get => busy; }


        public static void Connect()
        {
            connected = false;
            try
            {
                client = new TcpClient(address, port);
                stream = client.GetStream();

                string ans = ReceiveString();
                connected = (ans == "Connected!");
            }
            catch (Exception ex)
            {
                connected = false;
                Console.WriteLine(ex.Message);
                Disconnect();
            }
        }

        static Packet ReceivePacket()
        {
            try
            {
                byte[] data = new byte[sizeof(int)];
                int bytes = stream.Read(data, 0, sizeof(int));
                return (Packet)BitConverter.ToInt32(data, 0);
            }
            catch
            {
                Disconnect();
                return Packet.P_Error;
            }
        }

        static string ReceiveString()
        {
            try
            {
                StringBuilder response = new StringBuilder();
                response.Clear();

                byte[] msglen = new byte[sizeof(int)];
                stream.Read(msglen, 0, sizeof(int));

                int len = BitConverter.ToInt32(msglen, 0);

                byte[] msg = new byte[len];

                if (len == 0)
                    return "";

                stream.Read(msg, 0, len);

                response.Append(Encoding.UTF8.GetString(msg));
                return response.ToString();
            }
            catch
            {
                return "Error";
            }
        }

        static int ReceiveInt()
        {
            try
            {
                byte[] number = new byte[sizeof(int)];
                stream.Read(number, 0, sizeof(int));
                return BitConverter.ToInt32(number, 0);
            }
            catch
            {
                Disconnect();
                return -1;
            }
        }


        static void Send(Packet packet)
        {
            byte[] data;
            data = BitConverter.GetBytes((int)packet);
            stream.Write(data, 0, data.Length);
        }

        static void Send(int number)
        {
            byte[] data;
            data = BitConverter.GetBytes(number);
            stream.Write(data, 0, data.Length);
        }

        static void Send(string s)
        {
            Send(s.Length);
            byte[] data = Encoding.UTF8.GetBytes(s);
            stream.Write(data, 0, data.Length);
        }

        static void Send(FileStream file)
        {
            string s = "";
            
            using (StreamReader reader = new StreamReader(file, true))
            {
                s = reader.ReadToEnd();
            }

            Encoding encoding = Encoding.GetEncoding(1251);
            byte[] data = encoding.GetBytes(s);

            Send(data.Length);
            stream.Write(data, 0, data.Length);
        }

        static void ChangeString(ref string s)
        {
            char begin = (char)('a' - 1);
            char end = (char)(begin + 32);
            string newS = "";
            int i = 0;

            for (i = 0; i < s.Length; i++)
            {
                if (s[i] >= begin && s[i] <= end)
                    newS += (char)(s[i] + 'а' - begin);
                else
                    newS += s[i];
            }

            s = newS;
        }

        static void ProcessPacket(Packet packettype, ref string message)
        {
            try
            {
                switch (packettype)
                {
                    case Packet.P_ConnectionRequest:

                        connected = true;
                        return;
                    case Packet.P_FieldAnsRequest:

                        int english = ReceiveInt();
                        message = ReceiveString();

                        if (message == "Error")
                            return;

                        message += '\n';

                        int col_len = ReceiveInt();

                        if (english == 0)
                            ChangeString(ref message);

                        for (int i = 0; i < col_len; i++)
                        {
                            int pair = ReceiveInt();

                            char first = (char)(pair >> 16);
                            char second = (char)pair;
                            message += first;
                            message += second;
                        }

                        return;
                    case Packet.P_DictionaryAddAnsRequest:

                        int ans = ReceiveInt();
                        message = ans == 0 ? "Error" : "Good";
                        return;
                    default:
                        message = "Connection";
                        Console.WriteLine("Unrecognized packet");   
                        Disconnect();
                        return;
                }
            }
            catch
            {
                message = "Connection";
                Console.WriteLine("Подключение прервано!");
                Console.ReadLine();
                Disconnect();

                return;
            }
        }

        public static string GenerationRequest(int h, int w, int minL, int maxL, string dict)
        {
            busy = true;
            string message = "Error";
            try
            {
                Packet sendingPacket = Packet.P_FieldGenRequest;

                Send(sendingPacket);
                Send(h);
                Send(w);
                Send(minL);
                Send(maxL);
                Send(dict);

                Packet receivedPacket = ReceivePacket();
                ProcessPacket(receivedPacket, ref message);
            }
            catch
            {
                message = "Connection";
            }

            busy = false;
            return message;
        }


        public static void CheckConnetion()
        {
            busy = true;
            string message = "";
            try
            {
                Packet sendingPacket = Packet.P_ConnectionRequest;

                Send(sendingPacket);

                Packet receivedPacket = ReceivePacket();
                ProcessPacket(receivedPacket, ref message);
            }
            catch
            {
                connected = false;
                Disconnect();
            }
            busy = false;
        }


        public static string DictionaryAddRequest(string filename, string newName)
        {
            busy = true;
            string message = "Error";
            try
            {
                using (FileStream file = new FileStream(filename, FileMode.Open, FileAccess.Read))
                {
                    if (file.Length > 10485760)
                        return "Too large";

                    Console.WriteLine(file.Length);

                    Packet sendingPacket = Packet.P_DictionaryAddRequest;
                    
                    Send(sendingPacket);
                    Send(newName);

                    Packet receivedPacket = ReceivePacket();
                    if (receivedPacket == Packet.P_DictAlreadyExist)
                        return "Exist";

                    Send(file);

                    receivedPacket = ReceivePacket();
                    ProcessPacket(receivedPacket, ref message);
                }

                if (message == "Good")
                    return message;

                using (FileStream file = new FileStream(filename, FileMode.Open, FileAccess.Read))
                {
                    Packet sendingPacket = Packet.P_DictionaryAddRequest;

                    Send(sendingPacket);
                    Send(newName);

                    Packet receivedPacket = ReceivePacket();
                    if (receivedPacket == Packet.P_DictAlreadyExist)
                        return "Exist";

                    Send((int)file.Length);
                    for (long i = 0; i < file.Length; i++)
                        stream.WriteByte((byte)file.ReadByte());

                    receivedPacket = ReceivePacket();
                    ProcessPacket(receivedPacket, ref message);
                }
            }
            catch
            {
                message = "Connection";
                Console.WriteLine("Error while sending dictionary");
            }

            busy = false;
            return message;
        }

        public static string CancelGenerating()
        {
            busy = true;
            string message = "Error";
            try
            {
                Packet sendingPacket = Packet.P_StopGenRequest;
                Send(sendingPacket);
            }
            catch
            {
                message = "Error";
            }

            busy = false;
            return message;
        }


        public static string GetDictionaries()
        {
            busy = true;
            string message = "Error";
            try
            {
                Packet sendingPacket = Packet.P_DictionariesListRequest;
                Send(sendingPacket);

                message = ReceiveString();
            }
            catch
            {
                message = "Error";
            }

            busy = false;
            return message;
        }


        static void Disconnect()
        {
            if (stream != null)
                stream.Close();//отключение потока
            if (client != null)
                client.Close();//отключение клиента
            connected = false;
        }
    }
}