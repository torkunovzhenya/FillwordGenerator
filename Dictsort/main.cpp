#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

int main() {

    int n = 10;

    fstream fin, fout;
    fin.open("../Words/Dictionary.txt", ios::in);
    fout.open("../Words/" + to_string(n) + ".txt", ios::out);

    string word;
    vector<string> words;
    while (fin.good())
    {
        getline(fin, word);

        if (word.length() == n)
            words.push_back(word);
    }

    fout << words.size() << endl;
    for (const string& w : words)
        fout << w << endl;

    fin.close();
    fout.close();

    return 0;
}