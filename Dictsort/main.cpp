#include <iostream>
#include <fstream>
#include <map>

using namespace std;

int main() {
    fstream fin, fout;
    fin.open("word_rus.txt", ios::in);
    fout.open("10.txt", ios::out);

    map<int, int> m;
    string word;
    while (fin.good())
    {
        getline(fin, word);

        m[word.length()]++;
        if (word.length() >= 19)
            fout << word << endl;
    }

    int sum = 17161;
    for (int i = 0; i < 25; ++i) {
        cout << i << " " << m[i] << endl;
    }

    cout << sum;
    fin.close();
    fout.close();

    return 0;
}