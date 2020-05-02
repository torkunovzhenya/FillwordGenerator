#include <iostream>
#include "DancingLinks.hpp"

using namespace std;

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        cout << "Incorrect arguments format";
        return -1;
    }
    vector<int> figs = {3, 4, 5, 6, 7, 8};

    DancingLinks algo = DancingLinks(stoi(argv[1]), stoi(argv[2]), figs);
    algo.FindSolution();

    return 0;
}