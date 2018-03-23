/*
 * from rosettacode.org
 */
#pragma once

#include <vector>

std::vector<int> up_to(int n, int offset = 0)
{
    std::vector<int> retval(n);
    for (int ii = 0; ii < n; ++ii)
        retval[ii] = ii + offset;
    return retval;
}

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

    int largest_mobile() const	// returns 0 if no mobile integer exists
    {
        for (int r = values_.size(); r > 0; --r)
        {
            const int loc = positions_[r] + (directions_[r] ? 1 : -1);
            if (loc >= 0 && loc < values_.size() && values_[loc] < r)
                return r;
        }
        return 0;
    }

    bool complete() const { return largest_mobile() == 0; }

    void operator++()	// implement Johnson-Trotter algorithm
    {
        const int r = largest_mobile();
        const int rLoc = positions_[r];
        const int lLoc = rLoc + (directions_[r] ? 1 : -1);
        const int l = values_[lLoc];
        // do the swap
        std::swap(values_[lLoc], values_[rLoc]);
        std::swap(positions_[l], positions_[r]);
        sign_ = -sign_;
        // change directions
        for (auto pd = directions_.begin() + r + 1; pd != directions_.end(); ++pd)
            *pd = !*pd;
    }
};
