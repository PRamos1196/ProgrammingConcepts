#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <cctype>
#include <cstring>
#include <algorithm>
#include <iterator>
#include "tokens.h"
#include "parse.h"
#include <map>
#include <vector>
using namespace std;
map<string, Value> symbolMap;

void RunTimeError (string msg){
    cout << "0: RUNTIME ERROR " << msg << endl;
    exit(1);
}
int main(int argc, char* argv[])
{
    ifstream infile1;
    istream *in = &cin;
    int linenum = 0;
    if (argc == 2) {
        string arg(argv[1]);
        infile1.open(arg);
        if (infile1.is_open() == false)
        {
            cout << "COULD NOT OPEN " << arg << endl;
            return -1;
        }
        in = &infile1;
    }
    if (argc > 2) {
        cerr << "TOO MANY FILENAMES" << endl;
        return -1;
    }

    ParseTree *prog = Prog(in, &linenum);
    if (prog == 0)
    {
        return 0; // quit on error
    }
    prog->Eval(symbolMap);
    return 0;
}