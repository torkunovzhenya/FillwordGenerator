#include "DictionaryWorker.hpp"

using namespace std;


vector<string> getDictionaries()
{
    vector<string> ans;
    string path = "../Dictionaries/";

    char fullpath[MAX_PATH];
    GetFullPathName(path.c_str(), MAX_PATH, fullpath, nullptr);
    std::string fp(fullpath);

    WIN32_FIND_DATA findfiledata;
    HANDLE hFind = FindFirstFile((LPCSTR)(fp + "\\*").c_str(), &findfiledata);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (findfiledata.cFileName[0] != '.')
                ans.emplace_back(findfiledata.cFileName);
        }
        while (FindNextFile(hFind, &findfiledata) != 0);
    }

    return ans;
}


bool createDictionaryWords(const string& dict)
{
    string path = "../Dictionaries/" + dict + "/";
    fstream fin;
    fin.open(path + "Dictionary.txt", ios::in);

    if (!fin.is_open())
        return false;

    string word;
    vector<vector<string>> words = vector<vector<string>>(21);

    while (fin.good())
    {
        getline(fin, word);
        int len = word.length();
        vector<char> vector;
        for (int i = 0; i < len; ++i)
            vector.emplace_back(word[i]);
        if (word.length() <= 20)
            words[word.length()].push_back(word);
    }
    fin.close();

    for (int n = 3; n <= 20; ++n)
    {
        fstream fout;
        fout.open(path + to_string(n) + ".txt", ios::out);

        for (const string& w : words[n])
            fout << w << endl;

        fout.close();
    }

    fstream fout;
    fout.open(path + "counts.txt", ios::out);

    for (int i = 3; i <= 20; ++i)
        fout << words[i].size() << " ";

    fout.close();

    return true;
}


bool checkDictionary(const string& name)
{
    string s;
    fstream dict;
    dict.open("../Dictionaries/" + name + "/Dictionary.txt", ios::in);

    int diff = 'a' - 'A';
    char ch;
    bool flag = true;
    while (dict.get(ch))
    {
        if (ch >= 'A' && ch <= 'Z')
            s += to_string(ch + diff);
        else if ((ch >= 'a' && ch <= 'z') || ch == '\n')
            s += ch;
        else if (ch == ' ')
            continue;
        else
            flag = false;
    }
    dict.close();

    dict.open("../Dictionaries/" + name + "/Dictionary.txt", ios::out);
    dict << s;
    dict.close();
    return flag;
}