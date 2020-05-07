using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
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

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {

        }

        private void GenerationButton_Click(object sender, RoutedEventArgs e)
        {
            if (!Manager.Connect())
                return;

            h_selected = Combo_H.SelectedIndex + 3;
            w_selected = Combo_W.SelectedIndex + 3;
            minL_selected = Combo_MinL.SelectedIndex + 3;
            maxL_selected = Combo_MaxL.SelectedIndex + 3;
            GameWindow gameWindow = new GameWindow(h_selected, w_selected, minL_selected, maxL_selected);
            gameWindow.Show();
            this.Close();
        }
    }
}
