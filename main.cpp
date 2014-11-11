#include <iostream>
#include "buddy.h"
#include "command.h"
using namespace std;

void CheckArgNum(const vector<string>& items, const size_t& valid_num) {
    if (items.size() != valid_num) {
        cerr << "the number of arguments invalid. exit." << endl;
        exit(-1);
    }
}

bool HasSizeUnit(const vector<string>& items, const int& pos) {
    if (items[pos].find("KB") == string::npos
        && items[pos].find("MB") == string::npos
        && items[pos].find("GB") == string::npos
        && items[pos].find("TB") == string::npos) {
        cerr << "Some arguments have no unit. error and command ignored." << endl;
        return false;
    }
    return true;
}

int main() {
    BuddySystem *buddy_system = new BuddySystem();
    buddy_system->buddy_ = new MemNode();
    buddy_system->slab_ = new SlabMem();
    Commands *commands = new Commands();
    string input_line;
    while (getline(cin, input_line)) {
        commands->ReadCommand(input_line);
    }
    vector<string> process_items;
    for (int i = 0; i < commands->get_command_count(); ++i) {
        process_items.clear();
        process_items = commands->ParseCommand(i);
        if (process_items[0].compare("memorySize") == 0) {
            CheckArgNum(process_items, 2);
            if (!HasSizeUnit(process_items, 1))
                continue;
            buddy_system->buddy_->MemorySize(process_items[1]);
        }
        else if (process_items[0].compare("slabSize") == 0) {
            CheckArgNum(process_items, 3);
            if (!HasSizeUnit(process_items, 1)) {
                continue;
            }
            buddy_system->slab_->SlabSize(process_items[1], process_items[2], buddy_system->buddy_->mem_size_);
            buddy_system->Adjust();
        }
        else if (process_items[0].compare("alloc") == 0) {
            CheckArgNum(process_items, 3);
            if (!HasSizeUnit(process_items, 2)) {
                continue;
            }
            if (buddy_system->slab_->slab_node_ == NULL) {
                cerr << "Alloc should come after slabSize(). command ignored." << endl;
                continue;
            }
            MemNode* result_node = buddy_system->Alloc(stoi(process_items[1]), process_items[2]);
            if (result_node == NULL) {
                cerr << "Alloc failed for request: alloc(" << process_items[1] << ", " << process_items[2] << ")." << endl;
            }
        }
        else if (process_items[0].compare("realloc") == 0) {
            CheckArgNum(process_items, 3);
            if (!HasSizeUnit(process_items, 2)) {
                continue;
            }
            if (buddy_system->slab_->slab_node_ == NULL) {
                cerr << "Alloc should come after slabSize(). command ignored." << endl;
                continue;
            }
            MemNode* result_node = buddy_system->Realloc(stoi(process_items[1]), process_items[2]);
            if (result_node == NULL) {
                cerr << "Realloc failed for request: realloc(" << process_items[1] << ", " << process_items[2] << ")." << endl;
            }
        }
        else if (process_items[0].compare("free") == 0) {
            CheckArgNum(process_items, 2);
            if (buddy_system->slab_->slab_node_ == NULL) {
                cerr << "Alloc should come after slabSize(). command ignored." << endl;
                continue;
            }
            if (!buddy_system->Free(stoi(process_items[1]))) {
                cerr << "Free failed for request: free(" << process_items[1] << ")." << endl;
            }
        }
        else if (process_items[0].compare("dump") == 0) {
            CheckArgNum(process_items, 1);
            buddy_system->Dump();
        }
    }
    return 0;
}
