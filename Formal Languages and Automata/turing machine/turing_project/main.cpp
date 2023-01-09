#include "turing_machine.h"

int main(int argc, char* argv[]){
    if(argc >= 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)){
        printf("usage: turing [-v|--verbose] [-h|--help] <tm> <input>\n");
    }
    else if(argc >= 4 && (strcmp(argv[1], "--verbose") == 0 || strcmp(argv[1], "-v") == 0)){
        string tm_path(argv[2]);
        string input(argv[3]);
        turing_machine tm = turing_machine(tm_path, input, true);
        tm.operate(input, true);
    }
    else if(argc >= 3){
        string tm_path(argv[1]);
        string input(argv[2]);
        turing_machine tm = turing_machine(tm_path, input, false);
        tm.operate(input, false);
    }
    else{
        printf("error! wrong command! use: \"turing -h|--help\" to find out how to use this program.\n");
    }
}