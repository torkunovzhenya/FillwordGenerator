//
// Created by Евгений on 23.04.2020.
//

#include <iostream>
#include <fstream>
#include <ctime>
#include "DancingLinks.hpp"


Figure::Cell::Cell(int x, int y) : x(x), y(y) {}


Figure::Figure(int len, std::vector<std::pair<int, int>> nodes)
{
    if (len != 3)
        return;

    Cell* cell1 = new Cell(nodes[0].first, nodes[0].second);
    cells.push_back(cell1);

    Cell* cell2 = new Cell(nodes[1].first, nodes[1].second);
    cells.push_back(cell2);

    Cell* cell3 = new Cell(nodes[2].first, nodes[2].second);
    cells.push_back(cell3);

    cell1->neighbours.push_back(cell2);
    cell2->neighbours.push_back(cell1);

    cell2->neighbours.push_back(cell3);
    cell3->neighbours.push_back(cell2);
}


std::vector<char> Figure::GetRandomWay() {
    return std::vector<char>();
}


void Figure::FindWays() {

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


void DancingLinks::PrintRes()
{
    std::vector<std::vector<int>> field = std::vector<std::vector<int>>(height, std::vector<int>(width));

    for (int i = 0; i < res.size(); ++i)
        for (int cell : matrix->getFigureCells(res[i]))
            field[(cell - 1) / width][(cell - 1) % width] = i + 1;

    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
            std::cout << (char)(field[i][j] + 60) << " ";

        std::cout << std::endl;
    }
}


DancingLinks::~DancingLinks()
{
    delete matrix;
}
