//
// Created by pro on 2020/12/10.
//

#ifndef CPP_BITSET_H
#define CPP_BITSET_H

#include <string>
#include <vector>
#include <algorithm>

class Bitset {
    std::vector<unsigned int> A;
    unsigned int n;
public:
    std::string toString();
    Bitset(std::vector<unsigned int>&& _A, unsigned int _n): A(std::move(_A)), n(_n) {}
    Bitset(): n(0) {}
    Bitset(unsigned int n, bool c);
    int count() const;
    int size() const {return int(n);}
    void append(unsigned int k);
    Bitset operator | (const Bitset& x) const;
    Bitset operator & (const Bitset& x) const;
    Bitset operator ~ () const;
    Bitset exclude (const Bitset& x) const;
    bool checkCover(const Bitset& x) const;
    bool operator [] (unsigned int k) const;
    bool operator < (const Bitset& x) const;
};

#endif //CPP_BITSET_H
