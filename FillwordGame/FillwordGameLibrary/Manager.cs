using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Text;
using System.IO;

namespace FillwordGameLibrary
{
    enum Packet
    {
        P_ConnectionRequest = 0,
        P_FieldGenRequest,
        P_FieldAnsRequest,
        P_DictionaryAddRequest,
        P_DictionaryAddAnsRequest,
        P_Error
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

        static void Send(long number)
        {
            byte[] data;
            data = BitConverter.GetBytes(number);
            stream.Write(data, 0, data.Length);
        }

        static void Send(FileStream file)
        {
            for (long i = 0; i < file.Length; i++)
                stream.WriteByte((byte)file.ReadByte());
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
                        message = response.ToString();

                        if (message == "Error")
                            return true;

                        message += '\n';

                        stream.Read(msglen, 0, sizeof(int));
                        int col_len = BitConverter.ToInt32(msglen, 0);

                        Console.WriteLine(col_len);

                        ChangeString(ref message);
                        int prev_len = message.Length;

                        for (int i = 0; i < col_len; i++)
                        {
                            stream.Read(msglen, 0, sizeof(int));
                            int pair = BitConverter.ToInt32(msglen, 0);

                            char first = (char)(pair >> 16);
                            char second = (char)pair;
                            message += first;
                            message += second;
                        }

                        Console.WriteLine(message.Length - prev_len);
                        Console.WriteLine(message.Length);
                        
                        return true;
                    }
                    catch
                    {
                        message = "Error";
                        Console.WriteLine("Подключение прервано!");
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

            Send(sendingPacket);
            Send(h);
            Send(w);
            Send(minL);
            Send(maxL);

            Packet receivedPacket = ReceivePacket();

            ProcessPacket(receivedPacket, ref message);

            return message;
        }


        public static string DictionaryAddRequest(string filename)
        {
            string message = "";
            try
            {
                using (FileStream file = new FileStream(filename, FileMode.Open, FileAccess.Read))
                {
                    Packet sendingPacket = Packet.P_DictionaryAddRequest;
                    
                    Send(sendingPacket);
                    Send((int)file.Length);
                    Send(file);

                    Packet receivedPacket = ReceivePacket();

                    ProcessPacket(receivedPacket, ref message);
                }
            }
            catch
            {
                message = "Error";
                Console.WriteLine("Error while sending dictionary");
            }

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