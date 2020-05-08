#ifndef FILLWORDSERVER_DICTIONARYWORKER_HPP
#define FILLWORDSERVER_DICTIONARYWORKER_HPP

#include <windows.h>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

vector<string> getDictionaries();

bool createDictionaryWords(const string& dict);

void checkDictionary(const string& dict);

#endif //FILLWORDSERVER_DICTIONARYWORKER_HPP
