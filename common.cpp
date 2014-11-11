#include <string>
#include <iostream>

using namespace std;

void GetDigitsAndExponent(const string& size, int& digits, int& exponent) {
    int pos = 0;
    while (!isalpha(size[pos])) {
        ++pos;
    }
    digits = stoi(size.substr(0, pos));
    string unit = size.substr(pos);
    switch(unit[0]) {
        case 'K':
            exponent = 10;
            break;
        case 'M':
            exponent = 20;
            break;
        case 'G':
            exponent = 30;
            break;
        case 'T':
            exponent = 40;
            break;
    }
}

void GetDigitsAndUnit(const string& size, int& digits, string& unit) {
    int pos = 0;
    while (!isalpha(size[pos])) {
        ++pos;
    }
    digits = stoi(size.substr(0, pos));
    unit = size.substr(pos);
}

bool IsLessThanOrEqual(const string& size, const string& slab_size) {
    int digits1, digits2, exponent1, exponent2;
    GetDigitsAndExponent(size, digits1, exponent1);
    GetDigitsAndExponent(slab_size, digits2, exponent2);
    if (digits1 >= digits2) {
        double tmp_value;
        tmp_value = (double)digits1 / digits2;
        int minus = exponent2 - exponent1;
        if (minus >= 0 && tmp_value <= (1 << minus)) {
            return true;
        }
        else {
            return false;
        }
    }
    else {
        double tmp_value;
        tmp_value = (double)digits2 / digits1;
        int minus = exponent1 - exponent2;
        if (minus >= 0 && tmp_value >= (1 << minus)) {
            return true;
        }
        else {
            return false;
        }
    }
}

string Multiply2(const string& size) {
    int pos = 0, digits;
    string unit;
    while (!isalpha(size[pos])) {
        ++pos;
    }
    digits = stoi(size.substr(0, pos));
    unit = size.substr(pos);
    digits *= 2;
    if (digits >= 1024) {
        digits /= 1024;
        switch(unit[0]) {
            case 'K':
                unit[0] = 'M';
                break;
            case 'M':
                unit[0] = 'G';
                break;
            case 'G':
                unit[0] = 'T';
                break;
        }
    }
    return to_string((long long int)digits) + unit;
}

unsigned long long int NormalizedValue(const string& size) {
    int digits, exponent;
    GetDigitsAndExponent(size, digits, exponent);
    if (exponent != 10) {
        digits *= (1 << (exponent - 10));
    }
    return digits;
}

void Subtraction(unsigned long long int& digits, const string& second_size) {
    int digits2, exponent2;
    GetDigitsAndExponent(second_size, digits2, exponent2);
    unsigned long long int tmp_value;
    if (exponent2 != 10) {
        tmp_value = (unsigned long long int)digits2 * (1 << (exponent2 - 10));
    }
    else {
        tmp_value = (unsigned long long int)digits2;
    }
    digits -= tmp_value;
}

bool IsLargerThanHalf(const string& size, const string& mem_size) {
    string first_size = Multiply2(size);
    if (IsLessThanOrEqual(first_size, mem_size)) {
        return false;
    }
    else {
        return true;
    }
}

bool IsLessThan(const string& first_size, const string& second_size) {
    if (!IsLessThanOrEqual(first_size, second_size)) {
        return false;
    }
    else if (first_size == second_size) {
        return false;
    }
    else {
        return true;
    }
}

bool IsPower2(const string& size) {
    int digits;
    string unit;
    GetDigitsAndUnit(size, digits, unit);
    if (unit != "TB" && unit != "GB" && unit != "MB" && unit != "KB") {
        cerr << "the size has wrong unit. error and exit." << endl;
        exit(-1);
    }
    if ((digits & (digits - 1)) == 0) {
        return true;
    }
    else {
        return false;
    }
}


