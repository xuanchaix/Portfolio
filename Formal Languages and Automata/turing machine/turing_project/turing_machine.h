#include "libs.h"



class turing_machine{
public:
    turing_machine(string path, string input, bool verbose);
    void operate(string input, bool verbose);

private:
    int length;
    int half_length;
    int step;
    state cur_state;
    vector<int> header_lst;
    string path;
    vector<string> Q;
    vector<char> S;
    vector<char> G;
    string q0;
    char backspace;
    vector<string> F;
    int n;
    vector<vector<char>> tape_lst;
    map<state, delta> delta_map;

    delta find_possible_delta(bool verbose);
    void print_state();
};