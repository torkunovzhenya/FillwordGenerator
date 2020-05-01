#include <iostream>
#include <vector>
#include <set>
#include <fstream>

using namespace std;

struct figure
{
    int orientations = 1;
    bool reflection = false;
    int h, w;

    vector<pair<int, int>> cords;

    vector<figure> possible_variants;

    friend bool operator==(const figure& left, const figure& right) { return left.cords == right.cords; }

    friend bool operator!=(const figure& left, const figure& right) { return left.cords != right.cords; }

    friend bool operator<(const figure& left, const figure& right) { return left.cords < right.cords; }

    friend bool operator>(const figure& left, const figure& right) { return left.cords > right.cords; }
};


vector<vector<char>> field;
set<figure> figures_set;


void dfs(int x, int y, int x1, int y1, int x2, int y2)
{
    if (x > x1 && !field[x - 1][y])
    {
        field[x - 1][y] += 2;
        dfs(x - 1, y, x1, y1, x2, y2);
    }

    if (x < x2 && !field[x + 1][y])
    {
        field[x + 1][y] += 2;
        dfs(x + 1, y, x1, y1, x2, y2);
    }

    if (y > y1 && !field[x][y - 1])
    {
        field[x][y - 1] += 2;
        dfs(x, y - 1, x1, y1, x2, y2);
    }

    if (y < y2 && !field[x][y + 1])
    {
        field[x][y + 1] += 2;
        dfs(x, y + 1, x1, y1, x2, y2);
    }
}


// Function for checking if the figure contains empty places inside
bool Count_for_loops(int x1, int y1, int x2, int y2)
{
    dfs(x1, y1, x1, y1, x2, y2);

    int counter1 = 0;
    int counter2 = 0;
    for (int i = x1; i <= x2; ++i)
    {
        for (int j = y1; j <= y2; ++j)
        {
            if (!field[i][j])
                return false;

            if (field[i][j] == 2)
                field[i][j] = 0;
        }
    }

    return true;
}


void Add_to_set(int minx, int miny, int maxx, int maxy)
{
    if (!Count_for_loops(minx - 1, miny - 1, maxx + 1, maxy + 1))
        return;

    // Main position
    figure f1 = figure();
    for (int i = minx; i <= maxx; ++i)
        for (int j = miny; j <= maxy; ++j)
            if (field[i][j])
                f1.cords.emplace_back(i - minx, j - miny);

    if (figures_set.find(f1) != figures_set.end())
        return;

    // Rotate 90 degrees clockwise
    figure f2 = figure();
    for (int j = miny; j <= maxy; ++j)
        for (int i = maxx; i >= minx; --i)
            if (field[i][j])
                f2.cords.emplace_back(j - miny, maxx - i);

    // If they are equal - all rotates will be the same
    if (f2 != f1)
    {
        if (figures_set.find(f2) != figures_set.end())
            return;

        f1.orientations *= 2;
        f1.possible_variants.emplace_back(f2);

        // Rotate 180 degrees clockwise
        figure f3 = figure();
        for (int i = maxx; i >= minx; --i)
            for (int j = maxy; j >= miny; --j)
                if (field[i][j])
                    f3.cords.emplace_back(maxx - i, maxy - j);

        // If they are equal - there are 2 pairs of same orientations
        if (f3 != f1)
        {
            if (figures_set.find(f3) != figures_set.end())
                return;

            // Rotate 270 degrees clockwise
            figure f4 = figure();
            for (int j = maxy; j >= miny; --j)
                for (int i = minx; i <= maxx; ++i)
                    if (field[i][j])
                        f4.cords.emplace_back(maxy - j, i - minx);

            if (figures_set.find(f4) != figures_set.end())
                return;

            f1.orientations *= 2;
            f1.possible_variants.emplace_back(f3);
            f1.possible_variants.emplace_back(f4);
        }
    }

    // Reflection main position
    figure f5 = figure();
    for (int i = minx; i <= maxx; ++i)
        for (int j = maxy; j >= miny; --j)
            if (field[i][j])
                f5.cords.emplace_back(i - minx, maxy - j);

    // If it is equal to Main position - skip other positions
    if (f5 != f1)
    {
        if (figures_set.find(f5) != figures_set.end())
            return;

        if (f1.orientations == 1)
        {
            f1.possible_variants.emplace_back(f5);
            f1.reflection = true;
        }
        else
        {
            // Reflection rotate 90 degrees clockwise
            figure f6 = figure();
            for (int j = maxy; j >= miny; --j)
                for (int i = maxx; i >= minx; --i)
                    if (field[i][j])
                        f6.cords.emplace_back(maxy - j, maxx - i);

            if (f6 != f1)
            {
                if (figures_set.find(f6) != figures_set.end())
                    return;


                if (f1.orientations == 2)
                {
                    f1.reflection = true;
                    f1.possible_variants.emplace_back(f5);
                    f1.possible_variants.emplace_back(f6);
                }
                else
                {
                    // Reflection rotate 180 degrees clockwise
                    figure f7 = figure();
                    for (int i = maxx; i >= minx; --i)
                        for (int j = miny; j <= maxy; ++j)
                            if (field[i][j])
                                f7.cords.emplace_back(maxx - i, j - miny);

                    if (f7 != f1)
                    {
                        if (figures_set.find(f7) != figures_set.end())
                            return;

                        // Reflection rotate 270 degrees clockwise
                        figure f8 = figure();
                        for (int j = miny; j <= maxy; ++j)
                            for (int i = minx; i <= maxx; ++i)
                                if (field[i][j])
                                    f8.cords.emplace_back(j - miny, i - minx);
                        if (f8 != f1)
                        {
                            if (figures_set.find(f8) != figures_set.end())
                                return;

                            f1.reflection = true;
                            f1.possible_variants.emplace_back(f5);
                            f1.possible_variants.emplace_back(f6);
                            f1.possible_variants.emplace_back(f7);
                            f1.possible_variants.emplace_back(f8);
                        }
                    }
                }
            }
        }
    }

//    for (int i = minx; i <= maxx; ++i)
//    {
//        for (int j = miny; j <= maxy; ++j)
//            cout << (int)field[i][j] << " ";
//        cout << endl;
//    }
//    cout << "----------------------------------" << endl;


    // Adding to figures set, if there is no this figure
    f1.h = maxx - minx + 1;
    f1.w = maxy - miny + 1;
    figures_set.insert(f1);
}


void findWays(int x, int y, int left, string way, int minx, int miny, int maxx, int maxy, bool can_go_left)
{
    field[x][y] = 1;

    if (left == 0)
    {
        //cout << way << " " << maxx - minx + 1 << " " << maxy - miny + 1 << endl;

        Add_to_set(minx, miny, maxx, maxy);

        field[x][y] = 0;
        return;
    }

    // up and down
    if (!field[x - 1][y])
        findWays(x - 1, y, left - 1, way + '^', min(x - 1, minx), miny, maxx, maxy, can_go_left);
    if (!field[x + 1][y])
        findWays(x + 1, y, left - 1, way + 'v', minx, miny, max(x + 1, maxx), maxy, can_go_left);

    // left and right
    if (can_go_left && !field[x][y - 1])
        findWays(x, y - 1, left - 1, way + '<', minx, min(y - 1, miny), maxx, maxy, can_go_left);
    if (!field[x][y + 1])
        findWays(x, y + 1, left - 1, way + '>', minx, miny, maxx, max(y + 1, maxy), true);

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
        n = 8;
    else
        n = stoi(argv[1]);

    field = vector<vector<char>>(2 * n, vector<char>(2 * n));
    int x = n - 1;
    int y = n;

    field[n][n] = 1;
    field[x][y] = 1;

    fstream fout;
    fout.open("../Generated_figures/" + to_string(n) + ".txt", ios::out);

    if (fout.is_open())
    {
        findWays(x, y, n - 2, "^^", x, y, n, n, false);

        fout << figures_set.size() << endl;

        for (const figure& f : figures_set)
        {
            fout << f.orientations << " " << f.reflection << " " << f.h << " " << f.w << endl;

            for (auto cord : f.cords)
                fout << cord.first << " " << cord.second << " ";

            fout << endl;

            for (const figure& orientation : f.possible_variants)
            {
                for (auto cord : orientation.cords)
                    fout << cord.first << " " << cord.second << " ";
                fout << endl;
            }
        }
    }
    else
        cout << "Error while opening file!";

    fout.close();
    return 0;
}