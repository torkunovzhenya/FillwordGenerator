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
        if (word.length() <= 20)
            words[word.length()].push_back(word);
    }
    fin.close();

    for (int n = 3; n <= 20; ++n)
    {
        fstream fout;
        fout.open(path + to_string(n) + ".txt", ios::out);

        fout << words[n].size() << endl;
        for (const string& w : words[n])
            fout << w << endl;

        fout.close();
    }

    return true;
}


void checkDictionary(const string& dict)
{
    bool exist = false;
    for (const string& d : getDictionaries())
    {
        if (d == dict)
        {
            exist = true;
            break;
        }
    }

//    if (!exist)
//    {
        createDictionaryWords(dict);
        return;
//    }
}