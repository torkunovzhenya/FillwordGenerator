#include <iostream>
#include <vector>

using namespace std;

int minx, miny, maxx, maxy;

void findWays(int x, int y, int left, string way, vector<vector<char>>& field)
{
    if (left == 0)
    {
        cout << way << endl;
        return;
    }

    field[x][y] = 1;

    if (!field[x - 1][y])
        findWays(x - 1, y, left - 1, way + '^', field);
    if (!field[x + 1][y])
        findWays(x + 1, y, left - 1, way + 'v', field);
    if (!field[x][y - 1])
        findWays(x, y - 1, left - 1, way + '<', field);
    if (!field[x][y + 1])
        findWays(x, y + 1, left - 1, way + '>', field);

    field[x][y] = 0;
}


int main(int argc, char* argv[]) {

    if (argc > 2)
    {
        cout << "Incorrect number of arguments" << endl;
        return -1;
    }

    int n;
    if (argc == 1)
        n = 4;
    else
        n = stoi(argv[1]);

    vector<vector<char>> field = vector<vector<char>>(2 * n, vector<char>(2 * n));
    int x = n - 1;
    int y = n;

    field[n][n] = 1;
    field[x][y] = 1;

    findWays(x, y, n - 2, "^", field);

    return 0;
}