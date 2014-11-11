#include "buddy.h"
#include "common.cpp"
#include <iostream>
using namespace std;

MemNode::MemNode() {
    left_ = right_ = parent_ = NULL;
    lfree_ = rfree_ = self_free_ = true;
    pid_ = -1;
    mem_size_ = "";
}

int MemNode::get_pid() {
    return pid_;
}

void MemNode::set_pid(const int& user_id) {
    pid_ = user_id;
}

void MemNode::MemorySize(const string& size) {
    if (!IsPower2(size)) {
        cerr << "The input size of memorySize() is not a power of 2. error and exit." << endl;
        exit(-1);
    }
    mem_size_ = size;
}

bool BuddySystem::ExistedInMem(int pid) {
    //if (pid == -1) {
    //    cerr << "This is special number owned by slab allocator, please use other pid." << endl;
    //    return true;
    //}
    if (pid < 0 && pid != -10) { //magic number. should be optimized.
        cerr << "in ExistedInMem(): pid is less than 0. error." << endl;
        return false;
    }
    else if (begin_addr_.find(pid) != begin_addr_.end()) {
        return true;
    }
    else {
        return false;
    }
}

void BuddySystem::set_begin_addr(int pid, MemNode* position) {
    if (pid < 0) {
        cerr << "in set_begin_addr(): pid is less than 0. error." << endl;
        return;
    }
    begin_addr_[pid] = position;
}

SlabMem::SlabMem() {
    slab_node_ = NULL;
    slab_unit_ = total_size_ = "";
    cur_free_count_ = 0;
}

bool SlabMem::SlabSize(const string& size, const string& portion, const string& total_size) {
    if (!IsPower2(size)) {
        cerr << "The input size of slabSize() is not a power of 2. error and exit." << endl;
        exit(-1);
    }
    slab_unit_ = size;
    total_size_ = GetSlabTotalSize(total_size, portion);
    int digits1, digits2, exponent1, exponent2;
    GetDigitsAndExponent(total_size_, digits1, exponent1);
    GetDigitsAndExponent(slab_unit_, digits2, exponent2);
    cur_free_count_ = digits1 * (1 << (exponent1 - exponent2)) / digits2;
    return true;
}

string SlabMem::GetSlabTotalSize(const string& total_size, const string& portion) {
    string res, unit;
    int digits, pos;
    GetDigitsAndUnit(total_size, digits, unit);
    if (unit != "TB" && unit != "GB" && unit != "MB" && unit != "KB") {
        cerr << "the size has wrong unit. error and exit." << endl;
        exit(-1);
    }
    int numerator, denominator;
    pos = portion.find('/');
    numerator = stoi(portion.substr(0, pos));
    denominator = stoi(portion.substr(pos + 1));
    while (digits * numerator < denominator) {
        digits *= 1024;
        switch(unit[0]) {
            case 'T':
                unit[0] = 'G';
                break;
            case 'G':
                unit[0] = 'M';
                break;
            case 'M':
                unit[0] = 'K';
                break;
            case 'K':
                cerr << "there is not space to alloc to slab. error and exit." << endl;
                exit(-1);
        }
    }
    digits = digits * numerator / denominator;
    return to_string((long long int)digits) + unit;
}

BuddySystem::BuddySystem() {
    //buddy_ = new MemNode();
    //slab_ = new SlabMem();
    buddy_ = NULL;
    slab_ = NULL;
}

void BuddySystem::DumpHelper(const string& binary_str, MemNode* start_node) {
    if (start_node->self_free_) {
        cout << binary_str << " free" << endl;
        return;
    }
    if (!start_node->left_ && !start_node->right_) {
        cout << binary_str << " " << to_string((long long int)start_node->get_pid()) << endl;
        return;
    }
    if (start_node->left_ && start_node->left_->get_pid() != -10) {
        DumpHelper(binary_str + "0", start_node->left_);
    }
    if (start_node->right_) {
        DumpHelper(binary_str + "1", start_node->right_);
    }
}

void BuddySystem::Dump() {
    string binary_str("");
    cout << "slab:" << endl;
    string slab_total_size = slab_->total_size_;
    string buddy_total_size = buddy_->mem_size_;
    do {
        slab_total_size = Multiply2(slab_total_size);
        binary_str += "0";
    } while(IsLessThan(slab_total_size, buddy_total_size));
    DumpHelper(binary_str, slab_->slab_node_);
    cout << "buddy:" << endl;
    binary_str = "";
    DumpHelper(binary_str, buddy_);
}

void FindBestNode(const string& obj_size, MemNode* start_node, MemNode** best_node) {
    if (IsLessThanOrEqual(obj_size, start_node->mem_size_)) {
        if (start_node->self_free_) {
            if (IsLessThan(start_node->mem_size_, (*best_node)->mem_size_)) {
                *best_node = start_node;
            }
        }
        else {
            if (!start_node->lfree_ && start_node->left_->get_pid() != -10) {
                FindBestNode(obj_size, start_node->left_, best_node);
            }
            if (!start_node->rfree_) {
                FindBestNode(obj_size, start_node->right_, best_node);
            }
        }
    }
}

MemNode* BuddySystem::RealAlloc(const int& pid, const string& size, MemNode* start_node) {
    MemNode* best_node = start_node;
    FindBestNode(size, start_node, &best_node);
    MemNode* left_node, *right_node;
    int digits;
    string unit;
    while (!IsLargerThanHalf(size, best_node->mem_size_)) {
        best_node->self_free_ = best_node->lfree_ = best_node->rfree_ = false;
        best_node->left_ = new MemNode();
        best_node->right_ = new MemNode();
        left_node = best_node->left_;
        right_node = best_node->right_;
        left_node->parent_ = best_node;
        right_node->parent_ = best_node;
        GetDigitsAndUnit(best_node->mem_size_, digits, unit);
        if (digits == 1) {
            digits = 512;
            switch(unit[0]) {
                case 'T':
                    unit[0] = 'G';
                    break;
                case 'G':
                    unit[0] = 'M';
                    break;
                case 'M':
                    unit[0] = 'K';
                    break;
                default:
                    cerr << "memory split error. unit size blocked. error and exit." << endl;
                    exit(-1);
            }
        }
        else {
            digits /= 2;
        }
        string new_size = to_string((long long int)digits) + unit;
        left_node->mem_size_ = new_size;
        right_node->mem_size_ = new_size;
        best_node = left_node;
    }
    best_node->set_pid(pid);
    best_node->self_free_ = false;
    if (pid != -10) {
        set_begin_addr(pid, best_node);
    }
    Subtraction(total_free_space_, best_node->mem_size_);
    return best_node;
}

MemNode* FindFirstSlab(MemNode* slab_node) {
    if (!slab_node->left_ && !slab_node->right_ && slab_node->self_free_) {
        return slab_node;
    }
    if (slab_node->left_) {
        slab_node->self_free_ = false;
        MemNode* tmp = FindFirstSlab(slab_node->left_);
        if (tmp != NULL) {
            return tmp;
        }
    }
    if (slab_node->right_) {
        slab_node->self_free_ = false;
        MemNode* tmp = FindFirstSlab(slab_node->right_);
        if (tmp != NULL) {
            return tmp;
        }
    }
    return NULL;
}

MemNode* BuddySystem::RealSlabAlloc(const int& pid, const string& size, SlabMem* slab_) {
    if (slab_->cur_free_count_ == 0) {
        cerr << "there is no slab space to use." << endl;
        return NULL;
    }
    slab_->cur_free_count_--;
    MemNode* position = FindFirstSlab(slab_->slab_node_);
    position->set_pid(pid);
    position->self_free_ = false;
    set_begin_addr(pid, position);
    return position;
}

MemNode* BuddySystem::Alloc(const int& pid, const string& size) {
    if (ExistedInMem(pid)) {
        cerr << "pid existed. ignore the request." << endl;
        return NULL;
    }
    if (size[0] == '0') {
        cerr << "invalid size requested: " << size << ". error and exit." << endl;
        exit(-1);
    }
    if (total_free_space_ < NormalizedValue(size)) {
        cout << "there is no memory space to allocate. command ignored." << endl;
        return NULL;
    }
    if (IsLessThanOrEqual(size, slab_->slab_unit_)) {
        return RealSlabAlloc(pid, size, slab_);
    }
    else {
        return RealAlloc(pid, size, buddy_);
    }
}

void BuddySystem::PreAllocAllSlabs(MemNode* slab_node) {
    if (slab_node->mem_size_ == slab_->slab_unit_) {
        return;
    }
    MemNode* left_node, *right_node;
    int digits;
    string unit;
    slab_node->left_ = new MemNode();
    slab_node->right_ = new MemNode();
    left_node = slab_node->left_;
    right_node = slab_node->right_;
    left_node->parent_ = slab_node;
    right_node->parent_ = slab_node;
    GetDigitsAndUnit(slab_node->mem_size_, digits, unit);
    if (digits == 1) {
        digits = 512;
        switch(unit[0]) {
            case 'T':
                unit[0] = 'G';
                break;
            case 'G':
                unit[0] = 'M';
                break;
            case 'M':
                unit[0] = 'K';
                break;
            default:
                cerr << "memory split error. unit size blocked. error and exit." << endl;
                exit(-1);
        }
    }
    else {
        digits /= 2;
    }
    string new_size = to_string((long long int)digits) + unit;
    left_node->mem_size_ = new_size;
    right_node->mem_size_ = new_size;
    PreAllocAllSlabs(left_node);
    PreAllocAllSlabs(right_node);
}

void BuddySystem::Adjust() {
    total_free_space_ = NormalizedValue(buddy_->mem_size_);
    total_free_space_ -= NormalizedValue(slab_->total_size_);
    slab_->slab_node_ = Alloc(-10, slab_->total_size_);
    //pre-alloc all slabs
    PreAllocAllSlabs(slab_->slab_node_);
}

MemNode* BuddySystem::Realloc(int pid, const string& size) {
    if (!ExistedInMem(pid)) {
        cerr << "pid " << pid << " not existed so can't be reallocated. ignored." << endl;
        return NULL;
    }
    if (size[0] == '0') {
        cerr << "invalid size requested: " << size << ". error and exit." << endl;
        exit(-1);
    }
    if (!Free(pid)) {
        cerr << "pid " << pid << " can't be freed. error and exit." << endl;
        exit(-1);
    }
    return Alloc(pid, size);
    
}

void Merge(MemNode* node) {
    if (node->parent_ == NULL) {
        return;
    }
    if (node->self_free_ == false) {
        return;
    }
    if (node->parent_->lfree_ && node->parent_->rfree_) {
        MemNode* parent = node->parent_;
        parent->self_free_ = true;
        delete parent->left_;
        delete parent->right_;
        parent->left_ = NULL;
        parent->right_ = NULL;
        Merge(parent);
    }
}

bool BuddySystem::Free(int pid) {
    if (!ExistedInMem(pid)) {
        cerr << "pid " << pid << " not existed so can't be free. ignored." << endl;
        return false;
    }
    MemNode* target_node = begin_addr_[pid];
    target_node->set_pid(-1);
    target_node->self_free_ = true;
    begin_addr_.erase(pid);
    if (target_node->mem_size_ == slab_->slab_unit_) {
        slab_->cur_free_count_ = slab_->cur_free_count_ + 1;
    }
    else {
        if (target_node->parent_ != NULL) {
            if (target_node == target_node->parent_->left_) {
                target_node->parent_->lfree_ = true;
            }
            else {
                target_node->parent_->rfree_ = true;
            }
        }
        total_free_space_ += NormalizedValue(target_node->mem_size_);
        Merge(target_node);
    }
    return true;
}
