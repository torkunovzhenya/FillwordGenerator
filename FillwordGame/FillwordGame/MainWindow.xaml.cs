﻿using System;
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
        private static string dict_selected;
        private static int dict_index_selected = 0;

        private static string[] dicts = { "Russian1", "Russian2" };
        private static bool stopped = false;
        private static bool field_generating = false;

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

            for (int i = 3; i <= 10; i++)
            {
                TextBlock tmin = new TextBlock();
                tmin.Text = i.ToString();
                Combo_MinL.Items.Add(tmin);

                TextBlock tmax = new TextBlock();
                tmax.Text = i.ToString();
                Combo_MaxL.Items.Add(tmax);
            }

            UpdateDictionaries();

            Combo_H.SelectedIndex = h_selected - 3;
            Combo_W.SelectedIndex = w_selected - 3;
            Combo_MinL.SelectedIndex = minL_selected - 3;
            Combo_MaxL.SelectedIndex = maxL_selected - 3;
            Combo_Dict.SelectedIndex = dict_index_selected;
        }


        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            ConnectToServerAsync();
        }


        private async Task ConnectToServerAsync()
        {
            if (!Manager.Connected)
                await Task.Run(() => Manager.Connect());
        }


        private async Task GeneratingAsync()
        {
            generatingPlate.Visibility = Visibility.Visible;

            while (field_generating)
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


        private void UpdateDictionaries()
        {
            Combo_Dict.Items.Clear();
            foreach (string dict in dicts)
            {
                TextBlock tdict = new TextBlock();
                tdict.Text = dict;
                Combo_Dict.Items.Add(tdict);
            }

            Combo_Dict.SelectedIndex = 0;
        }


        private void DisableWindowElements()
        {
            generatingButton.IsEnabled = false;
            dictionaryAddButton.IsEnabled = false;
            updateListButton.IsEnabled = false;
            Combo_H.IsEnabled = false;
            Combo_W.IsEnabled = false;
            Combo_MinL.IsEnabled = false;
            Combo_MaxL.IsEnabled = false;
            Combo_Dict.IsEnabled = false;
        }

        private void EnableWindowElements()
        {
            generatingButton.IsEnabled = true;
            dictionaryAddButton.IsEnabled = true;
            updateListButton.IsEnabled = true;
            Combo_H.IsEnabled = true;
            Combo_W.IsEnabled = true;
            Combo_MinL.IsEnabled = true;
            Combo_MaxL.IsEnabled = true;
            Combo_Dict.IsEnabled = true;
        }


        private async void GenerationButton_Click(object sender, RoutedEventArgs e)
        {
            if (!Manager.Connected)
            {
                await ConnectToServerAsync();
                if (!Manager.Connected)
                    return;
            }

            h_selected = Combo_H.SelectedIndex + 3;
            w_selected = Combo_W.SelectedIndex + 3;
            minL_selected = Combo_MinL.SelectedIndex + 3;
            maxL_selected = Combo_MaxL.SelectedIndex + 3;
            dict_selected = ((TextBlock)Combo_Dict.SelectedItem).Text;
            dict_index_selected = Combo_Dict.SelectedIndex;

            if (maxL_selected < minL_selected)
            {
                MessageBox.Show("Minimal lenght can't be higher than maximal lenght!");
                return;
            }

            stopped = false;

            field_generating = true;
            DisableWindowElements();

            GeneratingAsync();
            string generated = await Task.Run(() => 
                Manager.GenerationRequest(h_selected, w_selected, minL_selected, maxL_selected, dict_selected));

            field_generating = false;
            EnableWindowElements();

            if (stopped)
                return;

            if (generated == "Error")
            {
                MessageBox.Show("Can't generate field with this parameters!");
                return;
            }

            GameWindow gameWindow = new GameWindow(h_selected, w_selected, minL_selected, maxL_selected, generated);
            gameWindow.Show();
            this.Close();
        }

        private void DictAddButton_Click(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.OpenFileDialog dlg = new Microsoft.Win32.OpenFileDialog();
            dlg.FileName = "Dictionary"; // Default file name
            dlg.DefaultExt = ".txt"; // Default file extension
            dlg.Filter = "Text documents (.txt)|*.txt"; // Filter files by extension

            // Show open file dialog box
            Nullable<bool> result = dlg.ShowDialog();

            // Process open file dialog box results
            if (result == true)
            {
                // Open document
                string filename = dlg.FileName;

                Manager.DictionaryAddRequest(filename);
            }
        }

        private void CancelButton_Click(object sender, RoutedEventArgs e)
        {
            stopped = true;
            field_generating = false;
            Manager.CancelGenerating();
        }

        private async void UpdateButton_Click(object sender, RoutedEventArgs e)
        {
            if (!Manager.Connected)
            {
                await ConnectToServerAsync();
                if (!Manager.Connected)
                    return;
            }

            string dicts_m = await Task.Run(() => Manager.GetDictionaries());
            dicts = dicts_m.Split(new char[] { '\n' }, StringSplitOptions.RemoveEmptyEntries);

            UpdateDictionaries();
        }
    }
}
