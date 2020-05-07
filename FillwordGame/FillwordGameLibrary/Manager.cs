using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Text;

namespace FillwordGameLibrary
{
    enum Packet
    {
        P_ConnectionRequest = 0,
        P_FieldGenRequest,
        P_FieldAnsRequest,
        P_DictionaryAddRequest
    };


    public static class Manager
    {
        const string address = "127.0.0.1";
        const int port = 1111;

        static TcpClient client = null;
        static NetworkStream stream = null;

        public static bool Connect()
        {
            bool connected = true;
            try
            {
                client = new TcpClient(address, port);
                stream = client.GetStream();

                StringBuilder response = new StringBuilder();
                byte[] msglen = new byte[sizeof(int)];

                stream.Read(msglen, 0, sizeof(int));
                int len = BitConverter.ToInt32(msglen, 0);

                Console.WriteLine(len);

                byte[] msg = new byte[len];

                int bytes = stream.Read(msg, 0, len);
                response.Append(Encoding.UTF8.GetString(msg, 0, bytes));

                connected = (response.ToString() == "Connected!");
            }
            catch (Exception ex)
            {
                connected = false;
                Console.WriteLine(ex.Message);
                Disconnect();
            }

            return connected;
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

        static bool ProcessPacket(Packet packettype, ref string message)
        {
            switch (packettype)
            {
                case Packet.P_FieldAnsRequest:
                    StringBuilder response = new StringBuilder();
                    try
                    {

                        response.Clear();
                        byte[] msglen = new byte[sizeof(int)];

                        stream.Read(msglen, 0, sizeof(int));
                        int len = BitConverter.ToInt32(msglen, 0);

                        Console.WriteLine(len);

                        byte[] msg = new byte[len];
                        int bytes = stream.Read(msg, 0, len);

                        response.Append(Encoding.UTF8.GetString(msg));
                        message = response.ToString() + '\n';

                        stream.Read(msglen, 0, sizeof(int));
                        int col_len = BitConverter.ToInt32(msglen, 0);

                        ChangeString(ref message);

                        for (int i = 0; i < col_len; i++)
                        {
                            stream.Read(msglen, 0, sizeof(int));
                            int pair = BitConverter.ToInt32(msglen, 0);

                            char first = (char)(pair >> 16);
                            char second = (char)pair;
                            message += first;
                            message += second;
                        }
                        
                        return true;
                    }
                    catch
                    {
                        message = "Error";
                        Console.WriteLine("Подключение прервано!"); //соединение было прервано
                        Console.ReadLine();
                        Disconnect();

                        return false;
                    }
                default:
                    Console.WriteLine("Unrecognized packet");
                    return false;
            }
        }

        public static string GenerationRequest(int h, int w, int minL, int maxL)
        {
            string message = "";
            StringBuilder response = new StringBuilder();

            Packet sendingPacket = Packet.P_FieldGenRequest;
            byte[] data;

            data = BitConverter.GetBytes((int)sendingPacket);
            stream.Write(data, 0, data.Length);
            
            data = BitConverter.GetBytes(h);
            stream.Write(data, 0, data.Length);
            data = BitConverter.GetBytes(w);
            stream.Write(data, 0, data.Length);
            data = BitConverter.GetBytes(minL);
            stream.Write(data, 0, data.Length);
            data = BitConverter.GetBytes(maxL);
            stream.Write(data, 0, data.Length);

            Packet packet;
            int bytes = stream.Read(data, 0, sizeof(int));

            ProcessPacket((Packet)BitConverter.ToInt32(data, 0), ref message);

            return message;
        }


        static void Disconnect()
        {
            if (stream != null)
                stream.Close();//отключение потока
            if (client != null)
                client.Close();//отключение клиента
            //Environment.Exit(0); //завершение процесса
        }
    }
}