#ifndef BUDDY_H
#define BUDDY_H 

#include <string>
#include <map>

using namespace std;

class MemNode {
public:
    MemNode();
    void MemorySize(const string& size);
    int get_pid();
    void set_pid(const int& user_id);
public:
    bool lfree_, rfree_, self_free_;
    MemNode *left_, *right_, *parent_;
    string mem_size_;
private:
    int pid_;
};

class SlabMem {
public:
    SlabMem();
    bool SlabSize(const string& size, const string& portion, const string& total_size);
    string GetSlabTotalSize(const string& total_size, const string& portion);
public:
    MemNode* slab_node_;
    string slab_unit_;
    string total_size_;
    unsigned long long int cur_free_count_;
};

class BuddySystem {
public:
    BuddySystem();
    void DumpHelper(const string& binary_str, MemNode* start_node);
    void Dump();
    void Adjust();
    MemNode* Alloc(const int& pid, const string& size);
    MemNode* RealAlloc(const int& pid, const string& size, MemNode* start_node);
    MemNode* RealSlabAlloc(const int& pid, const string& size, SlabMem* slab_);
    MemNode* Realloc(int pid, const string& size);
    void PreAllocAllSlabs(MemNode* slab_node);
    bool Free(int pid);
    void set_begin_addr(int pid, MemNode* position);
    bool ExistedInMem(int pid);
public:
    MemNode* buddy_;
    SlabMem* slab_;
    map<int, MemNode*> begin_addr_;
    unsigned long long int total_free_space_;
};

#endif
