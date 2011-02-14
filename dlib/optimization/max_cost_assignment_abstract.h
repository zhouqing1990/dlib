// Copyright (C) 2011  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#undef DLIB_MAX_COST_ASSIgNMENT_ABSTRACT_H__
#ifdef DLIB_MAX_COST_ASSIgNMENT_ABSTRACT_H__

#include "../matrix.h"
#include <vector>

namespace dlib
{

// ----------------------------------------------------------------------------------------

    template <typename EXP>
    typename EXP::type assignment_cost (
        const matrix_exp<EXP>& cost,
        const std::vector<long>& assignment
    );
    /*!
        requires
            - cost.nr() == cost.nc()
            - for all valid i:
                - 0 <= assignment[i] < cost.nr()
        ensures
            - Interprets cost as a cost assignment matrix. That is, cost(i,j) 
              represents the cost of assigning i to j.  
            - Interprets assignment as a particular set of assignments. That is,
              i is assigned to assignment[i].
            - returns the cost of the given assignment. That is, returns
              a number which is:
                sum over i: cost(i,assignment[i])
    !*/

// ----------------------------------------------------------------------------------------

    template <typename U, long NR, long NC, typename MM, typename L>
    std::vector<long> max_cost_assignment (
        const matrix<U,NR,NC,MM,L>& cost
    );
    /*!
        requires
            - U == some integer type (e.g. int)
            - cost.nr() == cost.nc()
        ensures
            - Finds and returns the solution to the following optimization problem:

                Maximize: f(A) == assignment_cost(cost, A)
                Subject to the following constraints:
                    - The elements of A are unique. That is, there aren't any 
                      elements of A which are equal.  
                    - A.size() == cost.nr()
    !*/

// ----------------------------------------------------------------------------------------

}

#endif // DLIB_MAX_COST_ASSIgNMENT_ABSTRACT_H__

