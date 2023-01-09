#include "turing_machine.h"

turing_machine::turing_machine(string path, string input, bool verbose){
    ifstream input_file;
    input_file.open(path, ios::in);
    string line;
    length = 1000000;
    half_length = length / 2;
    if(input_file.is_open()){
        //#Q
        getline(input_file, line);
        while(line.substr(0, 2) != "#Q"){
            getline(input_file, line);
        }
        line = delete_backspace(line);
        line = line.substr(4, line.size() - 5);
        line += ",";
        int i;
        while((i = line.find(',')) != string::npos){
            if(count(Q.begin(), Q.end(), line.substr(0, i)) == 0){
                Q.push_back(line.substr(0, i));
            }
            line = line.substr(i + 1, -1);
        }
        //#S
        getline(input_file, line);
        while(line.substr(0, 2) != "#S"){
            getline(input_file, line);
        }
        line = delete_backspace(line);
        line = line.substr(4, line.size() - 5);
        line += ",";
        while((i = line.find(',')) != string::npos){
            if(i == 1){
                if(count(S.begin(), S.end(), line[0]) == 0){
                    S.push_back(line[0]);
                }
                line = line.substr(i + 1, -1);
            }
            else{
                if(verbose)
                    fprintf(stderr, "#S input should be a char value but you have put %s\n", line.substr(0, i).c_str());
                else
                    fprintf(stderr, "syntax error\n");
                exit(1);
            }
        }
        //#G
        getline(input_file, line);
        while(line.substr(0, 2) != "#G"){
            getline(input_file, line);
        }
        line = delete_backspace(line);
        line = line.substr(4, line.size() - 5);
        line += ",";
        while((i = line.find(',')) != string::npos){
            if(i == 1){
                if(count(G.begin(), G.end(), line[0]) == 0){
                    G.push_back(line[0]);
                }
                line = line.substr(i + 1, -1);
            }
            else{
                if(verbose)
                    fprintf(stderr, "#G input should be a char value but you have put %s\n", line.substr(0, i).c_str());
                else
                    fprintf(stderr, "syntax error\n");
                exit(1);
            }
        }
        //#q0
        getline(input_file, line);
        while(line.substr(0, 3) != "#q0"){
            getline(input_file, line);
        }
        line = delete_backspace(line);
        q0 = line.substr(4, -1);
        if(count(Q.begin(), Q.end(), q0) != 1){
            if(verbose)
                fprintf(stderr, "#q0 input should in Q set but you have put %s\n", line.substr(0, -1).c_str());
            else
                fprintf(stderr, "syntax error\n");
            exit(1);
        }
        //#B
        getline(input_file, line);
        while(line.substr(0, 2) != "#B"){
            getline(input_file, line);
        }
        line = delete_backspace(line);
        line = line.substr(3, -1);
        if(line.size() == 1){
            backspace = line[0];
        }
        else{
            if(verbose)
                fprintf(stderr, "#B input should be a char value but you have put %s\n", line.substr(0, -1).c_str());
            else
                fprintf(stderr, "syntax error\n");
            exit(1);
        }
        //#F
        getline(input_file, line);
        while(line.substr(0, 2) != "#F"){
            getline(input_file, line);
        }
        line = delete_backspace(line);
        line = line.substr(4, line.size() - 5);
        line += ",";
        while((i = line.find(',')) != string::npos){
            if(count(F.begin(), F.end(), line.substr(0, i)) == 0){
                F.push_back(line.substr(0, i));
            }
            line = line.substr(i + 1, -1);
        }
        //#N
        getline(input_file, line);
        while(line.substr(0, 2) != "#N"){
            getline(input_file, line);
        }
        line = delete_backspace(line);
        line = line.substr(3, -1);
        n = stoi(line);
        if(n < 1){
            if(verbose)
                fprintf(stderr, "error! #N(num of tape) = %d < 1!", n);
            else
                fprintf(stderr, "syntax error\n");
            exit(1);
        }
        //delta
        while(getline(input_file, line)){
            string temp_line = line;
            delete_semi(line);
            i = line.find(' ');
            if(i != string::npos && count(Q.begin(), Q.end(), line.substr(0, i)) == 1){
                if(count(Q.begin(), Q.end(), line.substr(0, i)) == 0){
                    if(verbose)
                        fprintf(stderr, "Wrong transition functions! No state named %s in %s\n", line.substr(0, i).c_str(), temp_line.c_str());
                    else
                        fprintf(stderr, "syntax error\n");
                    exit(1);
                }
                line += " ";
                string cur_st = line.substr(0, i);
                line = line.substr(i + 1, -1);
                i = line.find(' ');
                string cur_tape = line.substr(0, i);
                line = line.substr(i + 1, -1);
                i = line.find(' ');
                string write_tape = line.substr(0, i);
                line = line.substr(i + 1, -1);
                i = line.find(' ');
                string move_direction = line.substr(0, i);
                line = line.substr(i + 1, -1);
                i = line.find(' ');
                string next_state = line.substr(0, i);
                line = line.substr(i + 1, -1);
                state st1;
                st1.cond = cur_st;
                if(n == cur_tape.size()){
                    for(int i = 0; i < n; i++){
                        st1.symbol_lst.push_back(cur_tape[i]);
                    }
                }
                else{
                    if(verbose)
                        fprintf(stderr, "wrong tape size %s in %s\n", cur_tape.c_str(), temp_line.c_str());
                    else
                        fprintf(stderr, "syntax error\n");
                    exit(1);
                }
                delta d1;
                if(n == write_tape.size()){
                    for(int i = 0; i < n; i++){
                        d1.write_in_symbol_lst.push_back(write_tape[i]);
                    }
                }
                else{
                    if(verbose)
                        fprintf(stderr, "wrong tape size %s in %s\n", write_tape.c_str(), temp_line.c_str());
                    else
                        fprintf(stderr, "syntax error\n");
                    exit(1);
                }
                if(n == move_direction.size()){
                    for(int i = 0; i < n; i++){
                        d1.direction_lst.push_back(move_direction[i]);
                    }
                }
                else{
                    if(verbose)
                        fprintf(stderr, "wrong tape size %s in %s\n", move_direction.c_str(), temp_line.c_str());
                    else
                        fprintf(stderr, "syntax error\n");
                    exit(1);
                }
                if(count(Q.begin(), Q.end(), next_state) == 1){
                    d1.next_cond = next_state;
                }
                else{
                    if(verbose)
                        fprintf(stderr, "Wrong transition functions! No state named %s in %s\n", next_state.c_str(), temp_line.c_str());
                    else
                        fprintf(stderr, "syntax error\n");
                    exit(1);
                }
                delta_map.insert(pair<state, delta>(st1, d1));
                
            }
            //else if(i == string::npos && (Q.begin(), Q.end(), line.substr(0, i)) == 0){
            //    printf("Wrong transition functions! No state named %s in %s\n", line.substr(0, i).c_str(), temp_line.c_str());
            //    exit(1);
            //}
        }
        //for(auto i: delta_map){
        //    state st1 = i.first;
        //    cout << st1.cond + " " << st1.symbol_lst[0] << st1.symbol_lst[1] << " " << delta_map[st1].write_in_symbol_lst[0] << delta_map[st1].write_in_symbol_lst[1] << " " << delta_map[st1].direction_lst[0] << delta_map[st1].direction_lst[1] << " " << delta_map[st1].next_cond << endl;
        //}
        //tape
        for(int i = 0; i < n; i++){
            tape_lst.push_back(vector<char>());
            for(int j = 0; j < length; j++){
                tape_lst[i].push_back(backspace);
            }
        }
        if(verbose)
            fprintf(stdout, "Input: %s\n", input.c_str());
        for(int i = 0; i < input.length(); i++){
            if(count(S.begin(), S.end(), input[i]) == 1){
                tape_lst[0][i + half_length] = input[i];
            }
            else{
                if(verbose){
                    fprintf(stderr, "==================== ERR ====================\n");
                    fprintf(stderr, "error: '%c' was not declared in the set of input symbols\n", input[i]);
                    fprintf(stderr, "Input: %s\n", input.c_str());
                    string point = "";
                    for(int j = 0; j < i + 7; j++){
                        point += " ";
                    }
                    point += "^";
                    fprintf(stderr, "%s", point.c_str());
                }
                else
                    fprintf(stderr, "illegal input\n");
                exit(1);
            }
        }
        if(verbose)
            fprintf(stdout, "==================== RUN ====================\n");
    }
    else{
        if(verbose)
            fprintf(stderr, "error! No file named %s in current dictionary.\n", path.c_str());
        else
            fprintf(stderr, "syntax error\n");
        exit(1);
    }
}

void turing_machine::operate(string input, bool verbose){
    cur_state.cond = q0;
    for(int i = 0; i < n; i++){
        header_lst.push_back(half_length);
        cur_state.symbol_lst.push_back(tape_lst[i][header_lst[i]]);
    }
    step = 0;
    while(count(F.begin(), F.end(), cur_state.cond) == 0){
        if(verbose){
            print_state();
        }
        if(step >= 1000000){
            print_state();
            fprintf(stderr, "error! Program didn't stop after 1000000 steps! Turing machine exited!\n");
            exit(1);
        }
        delta d1 = find_possible_delta(verbose);
        cur_state.cond = d1.next_cond;
        for(int i = 0; i < n; i++){
            if(d1.write_in_symbol_lst[i] != '*'){
                tape_lst[i][header_lst[i]] = d1.write_in_symbol_lst[i];
            }
            if(d1.direction_lst[i] == 'l'){
                header_lst[i]--;
            }
            else if(d1.direction_lst[i] == 'r'){
                header_lst[i]++;
            }
            cur_state.symbol_lst[i] = tape_lst[i][header_lst[i]];
        }
        ++step;
    }
    if(verbose){
        print_state();
    }
    int i = 0;
    while(tape_lst[0][i] == '_'){
        ++i;
    }
    int j = length - 1;
    while(tape_lst[0][j] == '_'){
        j--;
    }
    if(verbose){
        fprintf(stdout, "Result: ");
        for(int k = i; k <= j; k++){
            fprintf(stdout, "%c", tape_lst[0][k]);
        }
        printf("\n==================== END ====================\n");
    }
    else{
        for(int k = i; k <= j; k++){
            fprintf(stdout, "%c", tape_lst[0][k]);
        }
    }
}

delta turing_machine::find_possible_delta(bool verbose){
    delta d1;
    if(delta_map.count(cur_state)){
        d1 = delta_map[cur_state];
    }
    else{
        vector<state> matched_state;
        for(auto i: delta_map){
            if(i.first.cond == cur_state.cond){
                bool matched = true;
                for(int j = 0; j < i.first.symbol_lst.size(); j++){
                    if(i.first.symbol_lst[j] != '*' && i.first.symbol_lst[j] != cur_state.symbol_lst[j]){
                        matched = false;
                        break;
                    }
                }
                if(matched){
                    matched_state.push_back(i.first);
                }
            }
        }
        if(matched_state.size() == 0){
            int i = 0;
            while(tape_lst[0][i] == '_'){
                ++i;
            }
            int j = length - 1;
            while(tape_lst[0][j] == '_'){
                j--;
            }
            if(verbose){
                fprintf(stderr, "error! no matched delta function! Turing machine stoped!\n");
                fprintf(stdout, "current result: ");
                for(int k = i; k <= j; k++){
                    fprintf(stdout, "%c", tape_lst[0][k]);
                }
                printf("\n==================== END ====================\n");
            }
            else{
                for(int k = i; k <= j; k++){
                    fprintf(stdout, "%c", tape_lst[0][k]);
                }
            }
            exit(1);
        }
        int min_count = 100000000;
        state min_res;
        for(auto i: matched_state){
            int m = count(i.symbol_lst.begin(), i.symbol_lst.end(), '*');
            if(m < min_count){
                min_count = m;
                min_res = i;
            }
        }
        d1 = delta_map[min_res];
    }
    return d1;
}

void turing_machine::print_state(){
/*
Step   : 0
Index0 : 0 1 2 3 4 5 6
Tape0  : 1 0 0 1 0 0 1
Head0  : ^
Index1 : 0
Tape1  : _
Head1  : ^
State  : 0
---------------------------------------------
*/
    fprintf(stdout, "Step   : %d\n", step);
    for(int p = 0; p < n; p++){
        int i = 0;
        while(tape_lst[p][i] == '_'){
            ++i;
        }
        int j = length - 1;
        while(tape_lst[p][j] == '_'){
            j--;
        }
        if(i <= j){
            fprintf(stdout, "Index%d :", p);
            for(int k = min(i, header_lst[p]); k <= max(j, header_lst[p]); k++){
                fprintf(stdout, " %d", abs(k - half_length));
            }
            fprintf(stdout, "\nTape%d  :", p);
            for(int k = min(i, header_lst[p]); k <= max(j, header_lst[p]); k++){
                int t = abs(k - half_length) / 10;
                fprintf(stdout, " ");
                fprintf(stdout, "%c", tape_lst[p][k]);
                while(t > 0){
                    fprintf(stdout, " ");
                    t = t / 10;
                }
            }
            fprintf(stdout, "\nHead%d  :", p);
            for(int k = min(i, header_lst[p]); k < header_lst[p]; k++){
                int t = abs(k - half_length);
                if(t == 0){
                    t = 1;
                }
                fprintf(stdout, " ");
                while(t > 0){
                    fprintf(stdout, " ");
                    t = t / 10;
                }
            }
            fprintf(stdout, " ^\n");
        }
        else{
            fprintf(stdout, "Index%d : %d\n", p, abs(header_lst[p] - half_length));
            fprintf(stdout, "Tape%d  : _\n", p);
            fprintf(stdout, "Head%d  : ^\n", p);
        }
    }
    fprintf(stdout, "State  : %s\n", cur_state.cond.c_str());
    fprintf(stdout, "---------------------------------------------\n");
}