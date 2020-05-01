#include <iostream>
#include "DancingLinks.hpp"

using namespace std;

int main() {

    vector<int> figs = {4};

    DancingLinks algo = DancingLinks(4, 2, figs);
    algo.FindSolution();

    return 0;
}