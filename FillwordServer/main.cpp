#include <iostream>
#include "DancingLinks.hpp"

using namespace std;

int main() {

    vector<int> figs = {3, 4, 5, 6, 7, 8};

    DancingLinks algo = DancingLinks(20, 20, figs);
    algo.FindSolution();

    return 0;
}