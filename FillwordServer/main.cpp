#include <iostream>
#include "DancingLinks.hpp"

using namespace std;

int main() {

    vector<int> figs;
    figs.push_back(3);

    DancingLinks algo = DancingLinks(6, 7, figs);
    algo.FindSolution();

    return 0;
}