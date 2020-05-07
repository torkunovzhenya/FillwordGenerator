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

        public WordWrap(int number, string word)
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
            border.BorderThickness = new Thickness(2);
            border.BorderBrush = new SolidColorBrush(Color.FromRgb(204, 119, 34));
            border.Child = this;

            GameWindow.canvas.Children.Add(border);
            Canvas.SetLeft(border, 550);
            Canvas.SetTop(border, GameWindow.MARGIN_TOP + number * (this.Height + 10));
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
        private Border border;
        private TextBlock text;

        public bool IsActive { get => active; }

        public bool IsOpened { get => opened; }

        public int X { get => x; }

        public int Y { get => y; }

        public char Letter { get => letter; }

        public int ColorNum { get => color; }

        public Tile(int x, int y, char letter, char color)
        {
            this.x = x;
            this.y = y;
            this.letter = letter;
            this.color = color - 60;
            this.Background = new SolidColorBrush(Color.FromRgb(232, 172, 111));
            this.Height = GameWindow.TILE_H;
            this.Width = GameWindow.TILE_W;

            text = new TextBlock();
            text.Text = letter.ToString();
            text.Height = this.Height;
            text.Width = this.Width;
            text.FontSize = this.Height * 0.6 + 2;
            text.TextAlignment = TextAlignment.Center;

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
            this.Background = new SolidColorBrush(
                Color.FromRgb((byte)(color * 256 / GameWindow.n), 
                            (byte)(color * 256 / GameWindow.n), 
                            (byte)(color * 256 / GameWindow.n)));
            text.Foreground = (byte)(color * 256 / GameWindow.n) > 100 ? 
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

        public string CheckWay()
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
                        wordWrap.UpdateLayout();
                        break;
                    }
                }
                tiles.ForEach(tile => tile.Open());
                return reversed_word;
            }

            tiles.ForEach(tile => tile.Deactivate());
            return null;
        }
    }


    /// <summary>
    /// Логика взаимодействия для GameWindow.xaml
    /// </summary>
    public partial class GameWindow : Window
    {
        const int h = 10;
        const int w = 10;
        const int FIELD_SIDE = 500;
        public const int TILE_H = FIELD_SIDE / h;
        public const int TILE_W = FIELD_SIDE / w;
        public const int MARGIN_LEFT = 30;
        public const int MARGIN_TOP = 30;
        
        public static Canvas canvas;
        public static int n = 10;
        public static int WORD_H;
        public static int WORD_W;
        public static string[] words;
        public static int[] colorCounts;
        internal static List<WordWrap> wordWraps = new List<WordWrap>();

        private List<List<Tile>> field = new List<List<Tile>>();
        private TileWay way;
        private bool trackMouse = false;
        
        static string info =
            "м е т а з в ы к о д " +
            "н е н т я о л л а ш " +
            "к ш и о н м и к о р " +
            "а у е к о у т с п а " +
            "е б о г м л п р у м " +
            "и у л ь и я т е т с " +
            "н к у л с р о з р о " +
            "е ч ю л к г е р е р " +
            "а н у м м е ш ь н п " +
            "р а п а о к у м и е";
        static string color_info =
            "G G G G G I I = = = " +
            "G G G > A I I I I I " +
            "D D G > A J J F F I " +
            "D D G > A J J J F I " +
            "H D D D A J E E F I " +
            "H D D @ H J J E F F " +
            "H @ @ @ H J J E E F " +
            "H H H H H B B B E F " +
            "C C C C C B ? B E F " +
            "C < < < C C ? ? E E";

        public GameWindow()
        {
            InitializeComponent();
            
            string generated = Manager.GenerationRequest();
            connLabel.Content = generated;
            string[] game_info = generated.Split(new char[] { '\n', ' ' }, StringSplitOptions.RemoveEmptyEntries);
            n = int.Parse(game_info[0]);

            words = new string[n];
            for (int i = 0; i < n; i++)
                words[i] = game_info[i + 1];
            
            char[] letters = new char[w * h];
            char[] colors = new char[w * h];
            for (int i = 0; i < w * h; i++)
            {
                letters[i] = game_info[n + 1 + i][0];
                colors[i] = game_info[n + 1 + w * h + i][0];
            }

            n = words.Length;
            colorCounts = new int[n];
            WORD_H = (FIELD_SIDE + 10) / n - 10;
            WORD_W = 10 * words[n - 1].Length + 10;

            canvas = CanvasMap;

            for (int i = 0; i < n; i++)
                wordWraps.Add(new WordWrap(i, words[i]));

            for (int i = 0; i < h; i++)
            {
                field.Add(new List<Tile>());
                for (int j = 0; j < w; j++)
                {
                    colorCounts[colors[i * h + j] - 60]++;

                    Tile tile = new Tile(i, j, letters[(i * h + j) % letters.Length], colors[(i * h + j) % letters.Length]);
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

            if (col >= h || row >= w)
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
            way.CheckWay();
        }

        private void CanvasMap_MouseLeave(object sender, MouseEventArgs e)
        {
            if (!trackMouse)
                return;

            trackMouse = false;
            way.CheckWay();
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

            if (col >= h || row >= w)
                return;

            Tile tile = field[row][col];
            if (tile.IsOpened)
                return;

            if (tile.IsActive)
                way.ReturnTo(tile);
            else
                way.AddTile(tile);
        }
    }
}
