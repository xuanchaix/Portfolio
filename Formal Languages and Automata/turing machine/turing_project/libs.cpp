#include "libs.h"

string delete_backspace(string str){
    int i = 0;
    if(!str.empty()){
        while((i = str.find(' ', i)) != string::npos){
            str.erase(i, 1);
        }
        i = 0;
        while((i = str.find('\t', i)) != string::npos){
            str.erase(i, 1);
        }
        i = 0;
        if((i = str.find(';', i)) != string::npos){
            str.erase(i, -1);
        }
    }
    return str;
}

string delete_semi(string str){
    int i = 0;
    if(!str.empty()){
        if((i = str.find(';', i)) != string::npos){
            str.erase(i, -1);
        }
    }
    return str;
}

state::state(){

}

bool state::operator< (const state &rhs) const{
    if(this->cond < rhs.cond){
        return true;
    }
    else if(this->cond > rhs.cond){
        return false;
    }
    else{
        int i = 0;
        while(this->symbol_lst[i] == rhs.symbol_lst[i]){
            i++;
            if(i >= symbol_lst.size()){
                return false;
            }
        }
        return this->symbol_lst[i] < rhs.symbol_lst[i];
    }
};

delta::delta(){

}