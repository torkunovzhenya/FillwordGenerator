using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using FillwordGameLibrary;

namespace FillwordGame
{
    /// <summary>
    /// Логика взаимодействия для MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private static int h_selected = 10;
        private static int w_selected = 10;
        private static int minL_selected = 3;
        private static int maxL_selected = 8;
        private static bool connected = false;
        private static bool field_generated = false;

        public MainWindow()
        {
            InitializeComponent();

            for (int i = 3; i <= 25; i++)
            {
                TextBlock th = new TextBlock();
                th.Text = i.ToString();
                Combo_H.Items.Add(th);

                TextBlock tw = new TextBlock();
                tw.Text = i.ToString();
                Combo_W.Items.Add(tw);
            }

            for (int i = 3; i <= 15; i++)
            {
                TextBlock tmin = new TextBlock();
                tmin.Text = i.ToString();
                Combo_MinL.Items.Add(tmin);

                TextBlock tmax = new TextBlock();
                tmax.Text = i.ToString();
                Combo_MaxL.Items.Add(tmax);
            }

            Combo_H.SelectedIndex = h_selected - 3;
            Combo_W.SelectedIndex = w_selected - 3;
            Combo_MinL.SelectedIndex = minL_selected - 3;
            Combo_MaxL.SelectedIndex = maxL_selected - 3;
        }

        private async void Window_Loaded(object sender, RoutedEventArgs e)
        {
            await ConnectToServerAsync();
        }

        private async Task ConnectToServerAsync()
        {
            connected = await Task.Run(() => Manager.Connect());
        }

        private async Task Wait()
        {
            for (int i = 0; i < 100; i++)
            {
                Console.WriteLine(i);
            }
        }


        private async Task GeneratingAsync()
        {
            generatingPlate.Visibility = Visibility.Visible;

            while (!field_generated)
            {
                generatingLabel.Text = "Generating";

                for (int i = 0; i < 4; i++)
                {
                    await Task.Run(() => Thread.Sleep(400));
                    generatingLabel.Text += ".";
                }
            }

            generatingPlate.Visibility = Visibility.Hidden;
        }


        private async void GenerationButton_Click(object sender, RoutedEventArgs e)
        {
            if (!connected)
            {
                await ConnectToServerAsync();
                if (!connected)
                    return;
            }

            field_generated = false;

            h_selected = Combo_H.SelectedIndex + 3;
            w_selected = Combo_W.SelectedIndex + 3;
            minL_selected = Combo_MinL.SelectedIndex + 3;
            maxL_selected = Combo_MaxL.SelectedIndex + 3;

            GeneratingAsync();
            string generated = await Task.Run(() => 
                Manager.GenerationRequest(h_selected, w_selected, minL_selected, maxL_selected));

            field_generated = true;


            GameWindow gameWindow = new GameWindow(h_selected, w_selected, minL_selected, maxL_selected, generated);
            gameWindow.Show();
            this.Close();
        }
    }
}
