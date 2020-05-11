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
using System.Windows.Shapes;
using FillwordGameLibrary;

namespace FillwordGame
{
    class WordWrap : StackPanel
    {
        private string word;
        private Border border;
        private TextBlock text;

        public WordWrap(int number, string word, StackPanel wordsWrap)
        {
            this.Background = new SolidColorBrush(Color.FromRgb(232, 172, 111));
            this.word = word;
            this.Height = GameWindow.WORD_H;
            this.Width = GameWindow.WORD_W;

            text = new TextBlock();
            text.Text = word;
            text.Height = this.Height;
            text.Width = this.Width;
            text.FontSize = this.Height * 0.6 + 2;
            text.TextAlignment = TextAlignment.Center;

            this.Children.Add(text);

            border = new Border();
            border.Width = this.Width;
            border.BorderThickness = new Thickness(2);
            border.BorderBrush = new SolidColorBrush(Color.FromRgb(204, 119, 34));
            border.Child = this;
            
            wordsWrap.Children.Add(border);
            border.Margin = new Thickness(5);
        }

        public void CrossOut()
        {
            text.TextDecorations = TextDecorations.Strikethrough;
            this.Background = new SolidColorBrush(Color.FromRgb(200, 200, 200));
            text.UpdateLayout();
        }

        public bool CheckWord(string word)
        {
            return this.word == word;
        }
    }

    class Tile : StackPanel
    {
        private int x, y;
        private char letter;
        private bool active = false;
        private bool opened = false;
        private int color;
        private int color_num;
        private Border border;
        private TextBlock text;

        public bool IsActive { get => active; }

        public bool IsOpened { get => opened; }

        public int X { get => x; }

        public int Y { get => y; }

        public char Letter { get => letter; }

        public int ColorNum { get => color_num; }

        public Tile(int x, int y, char letter, char color)
        {
            this.x = x;
            this.y = y;
            this.letter = letter;
            this.color_num = color;
            this.color = color * 17569 % GameWindow.n;
            this.Background = new SolidColorBrush(Color.FromRgb(232, 172, 111));
            this.Height = GameWindow.TILE_H;
            this.Width = GameWindow.TILE_W;
            this.Orientation = Height > Width ? Orientation.Horizontal : Orientation.Vertical;

            text = new TextBlock();
            text.Text = letter.ToString();
            text.Width = this.Width;
            text.FontSize = Math.Min(this.Height, this.Width) * 0.6 + 2;
            text.TextAlignment = TextAlignment.Center;
            text.VerticalAlignment = VerticalAlignment.Center;
            text.HorizontalAlignment = HorizontalAlignment.Center;

            this.Children.Add(text);

            border = new Border();
            border.BorderThickness = new Thickness(2);
            border.BorderBrush = new SolidColorBrush(Color.FromRgb(204, 119, 34));
            border.Child = this;

            GameWindow.canvas.Children.Add(border);
            Canvas.SetLeft(border, GameWindow.MARGIN_LEFT + y * GameWindow.TILE_W);
            Canvas.SetTop(border, GameWindow.MARGIN_TOP + x * GameWindow.TILE_H);
        }

        public void Open()
        {
            opened = true;
            int full = 257;
            int gray = color * full / GameWindow.n;
            int delta = color * full % GameWindow.n;
            int delta_r = 0;
            int delta_g = 0;
            int delta_b = 0;

            if (3 * delta < GameWindow.n)
                delta_r = delta * full / GameWindow.n;
            else if (3 * delta > 2 * GameWindow.n)
                delta_g = delta * full / GameWindow.n;
            else
                delta_b = delta * full / GameWindow.n;


            this.Background = new SolidColorBrush(
                Color.FromRgb((byte)(gray + delta_r), 
                            (byte)(gray + delta_g), 
                            (byte)(gray + delta_b)));
            text.Foreground = (byte)(color * full / GameWindow.n) > 100 ? 
                Brushes.Black : Brushes.White;
        }

        public void Activate()
        {
            this.Background = Brushes.Wheat;
            active = true;
        }

        public void Deactivate()
        {
            this.Background = new SolidColorBrush(Color.FromRgb(232, 172, 111));
            active = false;
        }
    }

    class TileWay
    {
        private Stack<Tile> way = new Stack<Tile>();

        public TileWay(Tile firstTile)
        {
            firstTile.Activate();
            way.Push(firstTile);
        }

        public void ReturnTo(Tile tile)
        {
            while (way.Peek() != tile)
                way.Pop().Deactivate();
        }

        public void AddTile(Tile tile)
        {
            Tile last = way.Peek();
            if ((tile.X == last.X && Math.Abs(tile.Y - last.Y) == 1) ||
                (tile.Y == last.Y && Math.Abs(tile.X - last.X) == 1))
            {
                tile.Activate();
                way.Push(tile);
            }
        }

        public bool CheckWay()
        {
            bool sameColor = true;
            string reversed_word = "";
            List<Tile> tiles = new List<Tile>();
            while (way.Count != 0)
            {
                reversed_word += way.Peek().Letter;
                tiles.Add(way.Pop());

                if (tiles.Count != 1)
                    if (tiles[tiles.Count - 2].ColorNum != tiles[tiles.Count - 1].ColorNum)
                        sameColor = false;
            }

            char[] word_ch = reversed_word.ToCharArray();
            Array.Reverse(word_ch);
            string word = new string(word_ch);

            if (sameColor && 
                GameWindow.colorCounts[tiles[0].ColorNum] == word.Length && 
                GameWindow.words.Contains(word))
            {
                foreach (WordWrap wordWrap in GameWindow.wordWraps)
                {
                    if (wordWrap.CheckWord(word))
                    {
                        wordWrap.CrossOut();
                        break;
                    }
                }
                tiles.ForEach(tile => tile.Open());
                return true;
            }

            tiles.ForEach(tile => tile.Deactivate());
            return false;
        }
    }


    /// <summary>
    /// Логика взаимодействия для GameWindow.xaml
    /// </summary>
    public partial class GameWindow : Window
    {
        const int FIELD_SIDE = 500;
        public const int WORDS_IN_COL = 10;
        public const int MARGIN_LEFT = 30;
        public const int MARGIN_TOP = 30;
        
        public static Canvas canvas;
        public static int height;
        public static int width;
        public static int TILE_H;
        public static int TILE_W;
        public static int n;
        public static int WORD_H;
        public static int WORD_W;
        public static string[] words;
        public static int[] colorCounts;
        internal static List<WordWrap> wordWraps = new List<WordWrap>();

        private List<List<Tile>> field = new List<List<Tile>>();
        private TileWay way;
        private bool trackMouse = false;
        private int wordsLeft;
        private int minFigureLength;
        private int maxFigureLength;


        void PrepareField(int h, int w, int minL, int maxL, string generated, out char[] colors, out char[] letters)
        {
            wordWraps.Clear();

            height = h;
            TILE_H = FIELD_SIDE / height;

            width = w;
            TILE_W = FIELD_SIDE / width;

            minFigureLength = minL;
            maxFigureLength = maxL;

            n = int.Parse(generated.Substring(0, generated.IndexOf('\n')));

            wordsLeft = n;
            totalWordsLabel.Content = n;
            wordsLeftLabel.Content = n;

            string[] game_info = generated.Split(new char[] { '\n' }, n + 3);

            words = new string[n];
            for (int i = 0; i < n; i++)
                words[i] = game_info[i + 1];

            letters = new char[w * h];
            colors = new char[w * h];
            for (int i = 0; i < w * h; i++)
            {
                letters[i] = game_info[n + 1][i];
                colors[i] = game_info[n + 2][i];
            }
        }


        public GameWindow(int h, int w, int minL, int maxL, string generated)
        {
            InitializeComponent();

            PrepareField(h, w, minL, maxL, generated, out char[] colors, out char[] letters);
            
            colorCounts = new int[n];
            WORD_H = (FIELD_SIDE + 10) / WORDS_IN_COL - 10;
            WORD_W = 17 * (words[n - 1].Length) + 20;
            wordsWrap.Width = WORD_W + 20;

            canvas = CanvasMap;

            for (int i = 0; i < n; i++)
                wordWraps.Add(new WordWrap(i, words[i], wordsWrap));

            for (int i = 0; i < h; i++)
            {
                field.Add(new List<Tile>());
                for (int j = 0; j < w; j++)
                {
                    colorCounts[colors[i * w + j]]++;

                    Tile tile = new Tile(i, j, letters[i * w + j], colors[i * w + j]);
                    field[i].Add(tile);
                }
            }
        }

        private void CanvasMap_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            double x = Mouse.GetPosition(this).X;
            double y = Mouse.GetPosition(this).Y;

            if (x < MARGIN_LEFT || y < MARGIN_TOP)
                return;

            int col = (int)((x - MARGIN_LEFT - 3) / TILE_W);
            int row = (int)((y - MARGIN_TOP - 3) / TILE_H);

            if (col >= width || row >= height)
                return;

            Tile tile = field[row][col];
            if (tile.IsOpened)
                return;

            way = new TileWay(tile);
            trackMouse = true;
        }

        private void CanvasMap_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (!trackMouse)
                return;

            trackMouse = false;
            if (way.CheckWay())
            {
                wordsLeft--;
                wordsLeftLabel.Content = wordsLeft;
            }
        }

        private void CanvasMap_MouseLeave(object sender, MouseEventArgs e)
        {
            if (!trackMouse)
                return;

            trackMouse = false;
            if (way.CheckWay())
            {
                wordsLeft--;
                wordsLeftLabel.Content = wordsLeft;
            }
        }

        private void CanvasMap_MouseMove(object sender, MouseEventArgs e)
        {
            if (!trackMouse)
                return;

            double x = Mouse.GetPosition(this).X;
            double y = Mouse.GetPosition(this).Y;

            if (x < MARGIN_LEFT || y < MARGIN_TOP)
                return;

            int col = (int)((x - MARGIN_LEFT - 3) / TILE_W);
            int row = (int)((y - MARGIN_TOP - 3) / TILE_H);

            if (col >= width || row >= height)
                return;

            Tile tile = field[row][col];
            if (tile.IsOpened)
                return;

            if (tile.IsActive)
                way.ReturnTo(tile);
            else
                way.AddTile(tile);
        }

        private void MenuButton_Click(object sender, RoutedEventArgs e)
        {
            MessageBoxResult result = MessageBox.Show("Do you want to go to Menu?",
                                          "Confirmation",
                                          MessageBoxButton.YesNo,
                                          MessageBoxImage.Question);

            if (result == MessageBoxResult.No)
                return;

            MainWindow mainWindow = new MainWindow();
            mainWindow.Show();
            this.Close();
        }

        private void SolveButton_Click(object sender, RoutedEventArgs e)
        {
            MessageBoxResult result = MessageBox.Show("Do you want to see full solution?",
                                          "Confirmation",
                                          MessageBoxButton.YesNo,
                                          MessageBoxImage.Question);

            if (result == MessageBoxResult.No)
                return;
            
            for (int i = 0; i < height; i++)
                for (int j = 0; j < width; j++)
                    field[i][j].Open();
        }
    }
}
