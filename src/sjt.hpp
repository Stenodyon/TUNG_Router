/*
 * from rosettacode.org
 */
#pragma once

#include <vector>

std::vector<int> up_to(int n, int offset = 0);

struct permutation_iterator
{
    std::vector<int> values_;
    std::vector<int> positions_;	// size is n+1, first element is not used
    std::vector<bool> directions_;
    int sign_;

    permutation_iterator(int n)
        : values_(up_to(n, 1)),
        positions_(up_to(n + 1, -1)),
        directions_(n + 1, false), sign_(1)
    {}

    int largest_mobile() const;	// returns 0 if no mobile integer exists
    void operator++();	// implement Johnson-Trotter algorithm
    bool complete() const { return largest_mobile() == 0; }
};
