using System;
using System.Net.Sockets;
using System.Text;

namespace FillwordGameLibrary
{
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

        public static string ReceiveMessage()
        {
            string message = "";
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

                response.Append(Encoding.UTF8.GetString(msg, 0, bytes));
                message = response.ToString();
            }
            catch
            {
                message = "Error";
                Console.WriteLine("Подключение прервано!"); //соединение было прервано
                Console.ReadLine();
                Disconnect();
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