//
// Created by Евгений on 23.04.2020.
//

#include <iostream>
#include <fstream>
#include <ctime>
#include <algorithm>
#include <unordered_map>
#include "DancingLinks.hpp"


Figure::Cell::Cell(int x, int y) : x(x), y(y) {}


Figure::Figure(int minx, int miny, int maxx, int maxy, std::vector<std::pair<int, int>> cords)
{
    int h = maxx - minx + 1;
    int w = maxy - miny + 1;

    for (int i = 0; i < cords.size(); ++i)
    {
        cords[i].first -= minx;
        cords[i].second -= miny;
    }

    len = cords.size();
    std::vector<std::vector<Cell*>> figure_field =
            std::vector<std::vector<Cell*>>(h, std::vector<Cell*>(w, nullptr));

    for (auto cord : cords)
    {
        cells.push_back(new Cell(cord.first + minx, cord.second + miny));
        figure_field[cord.first][cord.second] = cells.back();
    }

    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j)
        {
            Cell* cell = figure_field[i][j];
            if (cell == nullptr)
                continue;

            if (i > 0 && figure_field[i - 1][j] != nullptr)
                cell->neighbours.push_back(figure_field[i - 1][j]);
            if (i < h - 1 && figure_field[i + 1][j] != nullptr)
                cell->neighbours.push_back(figure_field[i + 1][j]);
            if (j > 0 && figure_field[i][j - 1] != nullptr)
                cell->neighbours.push_back(figure_field[i][j - 1]);
            if (j < w - 1 && figure_field[i][j + 1] != nullptr)
                cell->neighbours.push_back(figure_field[i][j + 1]);
        }
}


void Figure::fill_figure(std::vector<Figure::Cell*>& way, const std::string& word)
{
    if (rand() % 2)
        for (int i = 0; i < word.length(); ++i)
            way[i]->letter = word[i];
    else
        for (int i = 0; i < word.length(); ++i)
            way[i]->letter = word[len - i - 1];
}


bool Figure::dfs(Figure::Cell *c, std::vector<Figure::Cell*>& ans)
{
    ans.push_back(c);
    c->visited = true;

    if (ans.size() == len)
        return true;

    std::vector<Cell*> neighbours;

    for (auto n : c->neighbours)
        if (!n->visited)
            neighbours.push_back(n);

    while (!neighbours.empty())
    {
        std::swap(neighbours[rand() % neighbours.size()], neighbours.back());

        if (dfs(neighbours.back(), ans))
            return true;

        neighbours.pop_back();
    }

    ans.pop_back();
    c->visited = false;

    return false;
}


std::vector<Figure::Cell*> Figure::GetRandomWay(const std::string& word)
{
    std::vector<Figure::Cell*> ans;

    for (Cell* cell : cells)
        if (cell->neighbours.size() == 1)
        {
            dfs(cell, ans);
            fill_figure(ans, word);
            return ans;
        }

    std::vector<Cell*> order;

    for (auto cell : cells)
        order.push_back(cell);

    while (!order.empty())
    {
        std::swap(order[rand() % order.size()], order.back());

        if (dfs(order.back(), ans))
        {
            fill_figure(ans, word);
            return ans;
        }

        order.pop_back();
    }
}


int Figure::getLen()
{
    return len;
}


bool operator<(const Figure &left, const Figure &right)
{
    return left.len < right.len;
}


std::pair<int, int> get_delta(int orient, int orientations, bool reflect, int h, int w)
{

    if (reflect && orient >= orientations)
    {
        if ((orient - orientations) % 2 == 0)
            return std::make_pair(w, h);

        return std::make_pair(h, w);
    }

    if (orient % 2 == 0)
        return std::make_pair(h, w);

    return std::make_pair(w, h);
}


DancingLinks::DancingLinks(int field_h, int field_w, int min_l, int max_l, const std::string& dict)
{
    srand((unsigned int)time(NULL));
    matrix = new LinkedMatrix(field_h, field_w);
    height = field_h;
    width = field_w;
    dictionary = dict;

    std::vector<int> lenghts;
    for (int l = min_l; l <= max_l; ++l)
        lenghts.push_back(l);

    std::fstream fin;
    fin.open("../Dictionaries/" + dict + "/counts.txt", std::ios::in);

    counts = std::vector<int>(21);
    for (int k = 3; k <= 20; ++k)
        fin >> counts[k];

    fin.close();

    for (int len : lenghts)
    {
        std::string file = "../../PolyminoGenerator/Generated_figures/" + std::to_string(len) + ".txt";
        std::fstream fin;
        fin.open(file, std::ios::in);

        if (!fin.is_open())
            continue;

        int n;
        fin >> n;

        std::vector<std::pair<int, int>> cords;
        for (int number = 0; number < n; ++number)
        {
            int orientations, reflection, h, w;
            fin >> orientations;
            fin >> reflection;
            fin >> h;
            fin >> w;

            orientations *= (reflection + 1);
            std::vector<int> cells;
            for (int orient = 0; orient < orientations; ++orient)
            {
                cords.clear();
                std::pair<int, int> delta = get_delta(orient, orientations, reflection, h, w);

                for (int l = 0; l < len; ++l)
                {
                    int x, y;
                    fin >> x;
                    fin >> y;
                    cords.emplace_back(x, y);
                }

                for (int i = 0; i <= field_h - delta.first; ++i)
                    for (int j = 0; j <= field_w - delta.second; ++j)
                    {
                        cells.clear();

                        for (auto cell : cords)
                            cells.push_back(field_w * (i + cell.first) + j + cell.second + 1);

                        matrix->AddLocation(cells);
                    }
            }
        }

        fin.close();
    }
}


bool DancingLinks::FindSolution()
{
    if (should_stop)
    {
        std::cout << "Sad story" << std::endl;
        return true;
    }

    Node* col = matrix->ChooseCell();
    if (col == nullptr)
    {
        std::cout << "Field successfully generated" << std::endl;
        return true;
    }
    if (matrix->isColumnEmpty(col))
        return false;

    Node* node = col;

    // Contain all the variants for current field cell
    std::vector<int> variants;
    std::vector<int> lenghts;
    while (node->down != col)
    {
        node = node->down;

        // Find the length of the figure
        int len = 0;
        Node* cell = node;
        while (cell->right != node)
        {
            cell = cell->right;
            ++len;
        }

        // If we cannot bring more figures with such length - skip
        if (counts[len] == 0)
            continue;

        variants.push_back(node->row);
        lenghts.push_back(len);
    }

    while (!variants.empty())
    {
        int rand_index = rand() % variants.size();
        int figure = variants[rand_index];

        // Hide rows with common columns with chosen
        auto rows_hidden = matrix->ChooseRow(figure);
        res.push_back(figure);
        counts[lenghts[rand_index]]--;

        // Recursively trying to find solution
        if (FindSolution())
            return true;

        // Restore hidden rows
        matrix->RestoreRows(rows_hidden);
        res.pop_back();
        counts[lenghts[rand_index]]++;

        std::swap(variants[rand_index], variants.back());
        std::swap(lenghts[rand_index], lenghts.back());
        variants.pop_back();
        lenghts.pop_back();
    }

    return false;
}


std::vector<std::string> DancingLinks::getWordsForField(std::vector<Figure>& figures)
{
    std::sort(figures.begin(), figures.end());
    std::vector<std::string> words;

    int n = figures.size();
    int i = 0;

    while (i < n)
    {
        int previ = i;
        int len = figures[i].getLen();

        // Enumerating figures with the same length
        while (i < n && figures[i].getLen() == len)
            ++i;

        // Open file with words with current len
        std::fstream fin;
        fin.open("../Dictionaries/" + dictionary + "/" + std::to_string(len) + ".txt", std::ios::in);

        // Get the number of words with current len
        int total_words = counts[len] + (i - previ);

        std::vector<int> variants;
        variants.reserve(total_words);
        for (int j = 0; j < total_words; ++j)
            variants.push_back(j);

        // Generating random indexes to read corresponding lines
        std::vector<int> genered_indexes;
        for (int gi = previ; gi < i; ++gi)
        {
            std::swap(variants[rand() % variants.size()], variants.back());
            genered_indexes.push_back(variants.back());
            variants.pop_back();
        }
        variants.clear();

        std::sort(genered_indexes.begin(), genered_indexes.end());
        std::string word;

        // Index of a line in file
        int str_num = 0;
        std::getline(fin, word);

        // Index of index in generated_indexes vector
        int word_index = 0;
        while (word_index < genered_indexes.size())
        {
            while (str_num < genered_indexes[word_index])
            {
                std::getline(fin, word);
                str_num++;
            }

            words.push_back(word);
            word_index++;
        }
        fin.close();
    }

    return words;
}


std::string DancingLinks::getRes(std::vector<int>& colors)
{
    std::vector<std::vector<char>> field =
            std::vector<std::vector<char>>(height, std::vector<char>(width));
    std::vector<std::vector<unsigned char>> solved_field =
            std::vector<std::vector<unsigned char>>(height, std::vector<unsigned char>(width));
    std::vector<Figure> figures;
    colors = std::vector<int>((height * width + 1) / 2);

    for (int res_figure : res) {
        int minx = height;
        int miny = width;
        int maxx = -1;
        int maxy = -1;

        std::vector<std::pair<int, int>> cords;
        for (int cell : matrix->getFigureCells(res_figure)) {
            int x = (cell - 1) / width;
            int y = (cell - 1) % width;
            cords.emplace_back(x, y);

            minx = std::min(minx, x);
            miny = std::min(miny, y);
            maxx = std::max(maxx, x);
            maxy = std::max(maxy, y);
        }

        figures.emplace_back(minx, miny, maxx, maxy, cords);
    }

    std::vector<std::string> words = getWordsForField(figures);

    int number = 0;
    for (int i = 0; i < figures.size(); ++i)
    {
        for (auto cell : figures[i].GetRandomWay(words[i]))
        {
            field[cell->x][cell->y] = cell->letter;
            solved_field[cell->x][cell->y] = (unsigned char)number;
        }
        number++;
    }

    std::fstream fout;
    fout.open("../Generated_field_with_words.txt", std::ios::out);
    std::string result = std::to_string(words.size()) + '\n';
    for (const std::string& word : words)
    {
        result += word + '\n';
        fout << word << std::endl;
    }

    fout << "----------------------------------";
    fout << std::endl;

    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            fout << field[i][j] << " ";
            result += field[i][j];
        }

        fout << std::endl;
    }

    fout << "----------------------------------";
    fout << std::endl;

    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            int num = i * width + j;
            if (num % 2 != 0)
                colors[num / 2] <<= 16;

            colors[num / 2] += solved_field[i][j];
            fout << solved_field[i][j] << " ";
        }

        fout << std::endl;
    }
    if (height * width % 2 == 1)
        colors.back() <<= 16;

    fout.close();
    return result;
}


DancingLinks::~DancingLinks()
{
    delete matrix;
}

void DancingLinks::Stop()
{
    should_stop = true;
}

bool DancingLinks::Stopped()
{
    return should_stop;
}
