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


DancingLinks::DancingLinks(int field_h, int field_w, std::vector<int> lenghts)
{
    srand((unsigned int)time(NULL));
    matrix = new LinkedMatrix(field_h, field_w);
    height = field_h;
    width = field_w;

    for (int len : lenghts)
    {
        std::string file = "../../PolyminoGenerator/Generated_figures/" + std::to_string(len) + ".txt";
        std::fstream fin;
        fin.open(file, std::ios::in);

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

//    if (lenghts.size() == 1 && lenghts[0] == 3)
//    {
//        std::vector<std::pair<int, int>> cords = std::vector<std::pair<int, int>>();
//        cords.push_back(std::make_pair(0, 0));
//        cords.push_back(std::make_pair(1, 0));
//        cords.push_back(std::make_pair(2, 0));
//        Figure* f1 = new Figure(3, cords);
//
//        cords.pop_back();
//        cords.push_back(std::make_pair(1, 1));
//        Figure* f2 = new Figure(3, cords);
//
//        cords.pop_back();
//        cords.pop_back();
//        cords.push_back(std::make_pair(0, 1));
//        cords.push_back(std::make_pair(1, 1));
//        Figure* f3 = new Figure(3, cords);
//
//        cords.pop_back();
//        cords.push_back(std::make_pair(1, 0));
//        Figure* f4 = new Figure(3, cords);
//
//        std::vector<int> cells;
//
//        for (int i = 0; i < field_h - 1; ++i)
//            for (int j = 0; j < field_w - 1; ++j)
//            {
//                cells.clear();
//
//                for (auto *cell : f2->cells)
//                    cells.push_back(field_w * (i + cell->x) + j + cell->y + 1);
//
//                matrix->AddLocation(cells);
//            }
//
//        for (int i = 0; i < field_h - 1; ++i)
//            for (int j = 0; j < field_w - 1; ++j)
//            {
//                cells.clear();
//
//                for (auto *cell : f3->cells)
//                    cells.push_back(field_w * (i + cell->x) + j + cell->y + 1);
//
//                matrix->AddLocation(cells);
//            }
//
//        for (int i = 0; i < field_h - 1; ++i)
//            for (int j = 0; j < field_w - 1; ++j)
//            {
//                cells.clear();
//
//                for (auto *cell : f4->cells)
//                    cells.push_back(field_w * (i + cell->x) + j + cell->y + 1);
//
//                matrix->AddLocation(cells);
//            }
//
//        for (int i = 0; i < field_h - 2; ++i)
//            for (int j = 0; j < field_w; ++j)
//            {
//                cells.clear();
//
//                for (auto *cell : f1->cells)
//                    cells.push_back(field_w * (i + cell->x) + j + cell->y + 1);
//
//                matrix->AddLocation(cells);
//            }
//        //Todo: Изменить тип фигур
//    }
}


bool DancingLinks::FindSolution()
{
    Node* col = matrix->ChooseCell();
    if (col == nullptr)
    {
        std::cout << "URRRRRRRAAAAA" << std::endl;
        PrintRes();
        return true;
    }
    if (matrix->isColumnEmpty(col))
    {
        //std::cout << "MEEEEEEEEEEH" << std::endl;
        return false;
    }

    Node* node = col;

    std::vector<Node*> variants;
    while (node->down != col)
    {
        node = node->down;
        variants.push_back(node);
    }
    while (!variants.empty())
    {
        int rand_index = rand() % variants.size();
        node = variants[rand_index];

        // Hide rows with common columns with chosen
        auto rows_hidden = matrix->ChooseRow(node->row);
        res.push_back(node->row);

        // Recursively trying to find solution
        if (FindSolution())
            return true;

        // Restore hidden rows
        matrix->RestoreRows(rows_hidden);
        res.pop_back();

        std::swap(variants[rand_index], variants.back());
        variants.pop_back();
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
        fin.open("../../Dictsort/Words/" + std::to_string(len) + ".txt", std::ios::in);

        // Reading the number of words
        int total_words;
        fin >> total_words;

        // Generating random indexes to read corresponding lines
        std::vector<int> genered_indexes;
        for (int gi = previ; gi < i; ++gi)
            genered_indexes.push_back(rand() % total_words);

        std::sort(genered_indexes.begin(), genered_indexes.end());

        // Reading the first line which is "\n"
        std::string word;
        std::getline(fin, word);

        // Index of a line in file
        int str_num = 0;

        // Index of index in generated_indexes vector
        int word_index = 0;
        while (word_index < genered_indexes.size())
        {
            if (word_index > 0 && genered_indexes[word_index - 1] == genered_indexes[word_index])
                genered_indexes[word_index]++;

            while (str_num <= genered_indexes[word_index])
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


void DancingLinks::PrintRes()
{
    std::vector<std::vector<char>> field = std::vector<std::vector<char>>(height, std::vector<char>(width));
    std::vector<Figure> figures;

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

    for (int i = 0; i < figures.size(); ++i)
        for (auto cell : figures[i].GetRandomWay(words[i]))
            field[cell->x][cell->y] = cell->letter;

    std::fstream fout;
    fout.open("../Generated_field_with_words.txt", std::ios::out);
    for (const std::string& word : words)
        fout << word << std::endl;

    fout << "-----------------------" << std::endl;

    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
            fout << field[i][j] << " ";

        fout << std::endl;
    }
}


DancingLinks::~DancingLinks()
{
    delete matrix;
}
