#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <algorithm>
#include <vector>
#include <map>
using namespace std;

class state{
public:
    state();
    string cond;
    vector<char> symbol_lst;
    bool operator <(const state &rhs) const;
};

class delta{
public:
    delta();
    vector<char> write_in_symbol_lst;
    vector<char> direction_lst;
    string next_cond;
};

string delete_backspace(string str);
string delete_semi(string str);