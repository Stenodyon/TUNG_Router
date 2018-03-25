#include "sjt.hpp"

std::vector<int> up_to(int n, int offset)
{
    std::vector<int> retval(n);
    for (int ii = 0; ii < n; ++ii)
        retval[ii] = ii + offset;
    return retval;
}

int permutation_iterator::largest_mobile() const	// returns 0 if no mobile integer exists
{
    for (int r = values_.size(); r > 0; --r)
    {
        const int loc = positions_[r] + (directions_[r] ? 1 : -1);
        if (loc >= 0 && loc < values_.size() && values_[loc] < r)
            return r;
    }
    return 0;
}

void permutation_iterator::operator++()	// implement Johnson-Trotter algorithm
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
