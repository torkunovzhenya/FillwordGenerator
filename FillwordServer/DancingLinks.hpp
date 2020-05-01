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
        int x, y;
    };

public:

    Figure(int length, std::vector<std::pair<int, int>> nodes);

    void FindWays();

    std::vector<char> GetRandomWay();

//private:

    std::vector<Cell*> cells;
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
