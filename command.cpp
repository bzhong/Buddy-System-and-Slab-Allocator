#include "command.h"
#include <sstream>
using namespace std;

Commands::Commands() {
    command_count = 0;
}

void Commands::ReadCommand(const string& line) {
    cmds_.push_back(line);
    ++command_count;
}

int Commands::get_command_count() {
    return command_count;
}

void Commands::RemoveWhiteSpace(vector<string>& elements) {
    for (size_t i = 0; i < elements.size(); ++i) {
        int begin = 0, end = elements[i].size() - 1;
        while (isspace(elements[i][begin])) {
            ++begin;
        }
        while (isspace(elements[i][end])) {
            --end;
        }
        elements[i] = elements[i].substr(begin, end - begin + 1);
    }
}

void Commands::ParseToElements(const string& command, vector<string>& elements) {
    stringstream ss(command);
    string token;
    while (getline(ss, token, ',')) {
        elements.push_back(token);
    }
    RemoveWhiteSpace(elements);
}

vector<string> Commands::ParseCommand(const int& pos) {
    string command = cmds_[pos];
    int begin = 0, end = command.size() - 1;
    while (isspace(command[begin])) {
        ++begin;
    }
    while (isspace(command[end])) {
        --end;
    }
    command = command.substr(begin, end - begin + 1);
    begin = command.find('(');
    string cmd_type = command.substr(0, begin);
    cmd_elements_.clear();
    cmd_elements_.push_back(cmd_type);
    end = command.find_last_of(")");
    command = command.substr(begin + 1, end - begin - 1);
    ParseToElements(command, cmd_elements_);
    return cmd_elements_;
}
