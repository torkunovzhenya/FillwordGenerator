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

    Figure(int minx, int miny, int maxx, int maxy, std::vector<std::pair<int, int>> cords);

    int getLen();

    std::vector<Cell*> GetRandomWay(const std::string& word);

private:

    int len;
    std::vector<Cell*> cells;

    bool dfs(Cell* c, std::vector<Figure::Cell*>& ans);

    void fill_figure(std::vector<Cell*>& way, const std::string& word);

    friend bool operator<(const Figure& left, const Figure& right);
};


class DancingLinks
{

public:

    DancingLinks(int field_h, int field_w, int min_l, int max_l, const std::string& dict);

    bool FindSolution();

    std::string getRes(std::vector<int>& colors);

    void Stop();

    bool Stopped();

    ~DancingLinks();

private:

    int height;
    int width;
    bool should_stop = false;
    LinkedMatrix* matrix = nullptr;
    std::vector<int> res;
    std::vector<int> counts;
    std::string dictionary;

    std::vector<std::string> getWordsForField(std::vector<Figure>& figures);
};


#endif //FILLWORDSERVER_DANCINGLINKS_HPP
