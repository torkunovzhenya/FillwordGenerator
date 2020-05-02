//
// Created by Евгений on 23.04.2020.
//

#ifndef FILLWORDSERVER_DANCINGLINKS_HPP
#define FILLWORDSERVER_DANCINGLINKS_HPP

#include <vector>
#include "LinkedMatrix.hpp"

class Figure
{
    class Cell
    {
    public:
        Cell(int x, int y);

        std::vector<Cell*> neighbours;
        bool visited = false;
        int x, y;
        char letter;
    };

public:

    Figure(int h, int w, const std::vector<std::pair<int, int>>& nodes);

    std::vector<Cell*> GetRandomWay(const std::string& word);

private:

    int len;

    std::vector<Cell*> cells;

    void fill_figure(std::vector<Cell*>& way, const std::string& word);

    static bool dfs(Cell* c, std::vector<Figure::Cell*>& ans);
};


class DancingLinks
{

public:

    DancingLinks(int field_h, int field_w, std::vector<int> lenghts);

    bool FindSolution();

    void PrintRes();

    virtual ~DancingLinks();

private:

    int height;
    int width;
    LinkedMatrix* matrix = nullptr;
    std::vector<int> res;
};


#endif //FILLWORDSERVER_DANCINGLINKS_HPP
