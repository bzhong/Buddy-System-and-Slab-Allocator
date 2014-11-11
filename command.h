#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <vector>
using namespace std;

class Commands {
public:
    Commands();
    ~Commands() {}
    void ReadCommand(const string& line);
    int get_command_count();
    void RemoveWhiteSpace(vector<string>& elements);
    vector<string> ParseCommand(const int& pos);
    void ParseToElements(const string& command, vector<string>& elements);
private:
    vector<string> cmds_;
    int command_count;
    vector<string> cmd_elements_;
};

#endif
