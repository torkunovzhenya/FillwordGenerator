#include "DictionaryWorker.hpp"

using namespace std;

// Get the list of all directories in directory Dictionaries
vector<string> getDictionaries()
{
    vector<string> ans;
    string path = "../Dictionaries/";

    char fullpath[MAX_PATH];
    GetFullPathName(path.c_str(), MAX_PATH, fullpath, nullptr);
    std::string fp(fullpath);

    // Get the first file
    WIN32_FIND_DATA findfiledata;
    HANDLE hFind = FindFirstFile((LPCSTR)(fp + "\\*").c_str(), &findfiledata);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            // If current file is dictionary - add to ans
            if ((findfiledata.dwFileAttributes | FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY
                && (findfiledata.cFileName[0] != '.'))
                ans.emplace_back(findfiledata.cFileName);
        }
        while (FindNextFile(hFind, &findfiledata) != 0);
    }

    return ans;
}


bool createDictionaryWords(const string& dict)
{
    string path = "../Dictionaries/" + dict + "/";
    wfstream fin;
    fin.open(path + "Dictionary.txt", ios::in);

    if (!fin.is_open())
        return false;

    wstring word;
    vector<vector<wstring>> words = vector<vector<wstring>>(21);

    while (fin.good())
    {
        getline(fin, word);
        int len = word.length();

        if (len <= 20)
            words[len].push_back(word);
    }
    fin.close();

    for (int n = 3; n <= 20; ++n)
    {
        wfstream fout;
        fout.open(path + to_string(n) + ".txt", ios::out);

        for (const wstring& w : words[n])
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


bool alreadyExist(const string& name)
{
    for (const string& dict: getDictionaries())
        if (dict == name)
            return true;

    return false;
}


bool checkDictionary(const string& name)
{
    wfstream dict;
    dict.open("../Dictionaries/" + name + "/Dictionary.txt", ios::in);

    if (!dict.is_open())
        return false;

    int diff_Eng = 'a' - 'A';
    int diff_Rus = L'а' - L'А';
    bool flag = true;
    bool rus = false;
    bool eng = false;

    wchar_t ch;
    wstring s;

    while (dict.get(ch))
    {
        if (ch == ' ')
            continue;
        else if (ch == '\n')
            s += ch;
        else if (ch >= 'A' && ch <= 'Z')
        {
            s += ch + diff_Eng;
            eng = true;
        }
        else if ((ch >= 'a' && ch <= 'z'))
        {
            s += ch;
            eng = true;
        }
        else if (ch >= L'А' && ch <= L'Я')
        {
            s += ch + diff_Rus;
            rus = true;
        }
        else if ((ch >= L'а' && ch <= L'я'))
        {
            s += ch;
            rus = true;
        }
        else if (ch == L'ё' || ch == L'Ё')
        {
            s += L'е';
            rus = true;
        }
        else
            flag = false;
    }
    dict.close();

    dict.open("../Dictionaries/" + name + "/Dictionary.txt", ios::out);

    if (!dict.is_open() || (rus == eng))
        flag = false;

    dict << s;
    dict.close();

    return flag;
}