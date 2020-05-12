#ifndef FILLWORDSERVER_DICTIONARYWORKER_HPP
#define FILLWORDSERVER_DICTIONARYWORKER_HPP

#include <windows.h>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

class DictionaryWorker
{
public:

    static vector<string> getDictionaries();

    static bool createDictionaryWords(const string& dict);

    static bool alreadyExist(const string& name);

    static bool checkDictionary(const string& dict);
};

#endif //FILLWORDSERVER_DICTIONARYWORKER_HPP
