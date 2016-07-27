///
// myalgs.hpp
//
// Extension to <algorithm> of <StandardCplusplus.h> for Arduino
//
#ifndef MYALGS_HPP
#define MYALGS_HPP

namespace std
{
    template<class InputIterator, class UnaryPredicate>
    bool all_of(InputIterator first, InputIterator last, UnaryPredicate pred);

    template<class InputIterator, class UnaryPredicate>
    bool any_of(InputIterator first, InputIterator last, UnaryPredicate pred);

    template<class InputIterator, class UnaryPredicate>
    bool none_of(InputIterator first, InputIterator last, UnaryPredicate pred);
}

#include "myalgs.cpp"  // implementations defined here

#endif  // MYALGS_HPP
