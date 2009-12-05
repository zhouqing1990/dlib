// Copyright (C) 2009  Davis E. King (davisking@users.sourceforge.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#undef DLIB_EMPIRICAL_KERNEl_MAP_ABSTRACT_H_
#ifdef DLIB_EMPIRICAL_KERNEl_MAP_ABSTRACT_H_

#include <vector>
#include "../matrix.h"
#include "kernel_abstract.h"
#include <vector>

namespace dlib
{

// ----------------------------------------------------------------------------------------

    template <
        typename kern_type
        >
    class empirical_kernel_map
    {
        /*!
            REQUIREMENTS ON kern_type
                - must be a kernel function object as defined in dlib/svm/kernel_abstract.h

            INITIAL VALUE
                - out_vector_size() == 0

            WHAT THIS OBJECT REPRESENTS
                This object represents a map from objects of sample_type (the kind of object 
                the kernel function operates on) to finite dimensional column vectors which 
                represent points in the kernel feature space defined by whatever kernel 
                is used with this object. 

                More precisely, to use this object you supply it with a particular kernel and 
                a set of basis samples.  After that you can present it with new samples and it 
                will project them into the part of kernel feature space spanned by your basis 
                samples.   
                
                This means the empirical_kernel_map is a tool you can use to very easily kernelize 
                any algorithm that operates on column vectors.  All you have to do is select a 
                set of basis samples and then use the empirical_kernel_map to project all your 
                data points into the part of kernel feature space spanned by those basis samples.
                Then just run your normal algorithm on the output vectors and it will be effectively 
                kernelized.  

                Regarding methods to select a set of basis samples, if you are working with only a 
                few thousand samples then you can just use all of them as basis samples.  
                Alternatively, the linearly_independent_subset_finder often works well for 
                selecting a basis set.  Some people also find that picking a random subset 
                works fine.
        !*/

    public:

        typedef kern_type kernel_type;
        typedef typename kernel_type::sample_type sample_type;
        typedef typename kernel_type::scalar_type scalar_type;
        typedef typename kernel_type::mem_manager_type mem_manager_type;

        struct empirical_kernel_map_error : public error;
        /*!
            This is an exception class used to indicate a failure to create a 
            kernel map from data given by the user.
        !*/

        template <typename EXP>
        void load(
            const kernel_type& kernel,
            const matrix_exp<EXP>& basis_samples
        );
        /*!
            requires
                - is_vector(basis_samples) == true
                - basis_samples.size() > 0
                - kernel must be capable of operating on the elements of basis_samples.  That is,
                  expressions such as kernel(basis_samples(0), basis_samples(0)) should make sense.
            ensures
                - 0 < #out_vector_size() <= basis_samples.size()
                - #get_kernel() == kernel
                - This function constructs a map between normal sample_type objects and the 
                  subspace of the kernel feature space defined by the given kernel and the
                  given set of basis samples.  So after this function has been called you
                  will be able to project sample_type objects into kernel feature space
                  and obtain the resulting vector as a normal column matrix.
            throws
                - empirical_kernel_map_error
                    This exception is thrown if we are unable to create a kernel map.
                    If this happens then this object will revert back to its initial value.
        !*/

        void load(
            const kernel_type& kernel,
            const std::vector<sample_type>& basis_samples
        );
        /*!
            requires
                - basis_samples.size() > 0
            ensures
                - performs load(kernel,vector_to_matrix(basis_samples)).  I.e. This function
                  does the exact same thing as the above load() function but lets you use
                  a std::vector of basis samples in addition to a row/column matrix of basis samples.
        !*/

        const kernel_type get_kernel (
        ) const;
        /*!
            requires
                - out_vector_size() != 0
            ensures
                - returns a copy of the kernel used by this object
        !*/

        long out_vector_size (
        ) const;
        /*!
            ensures
                - if (this object has been loaded with sample basis functions) then
                    - returns the dimensionality of the space the kernel map projects
                      new data samples into via the project() function.
                - else
                    - returns 0
        !*/

        const matrix<scalar_type,0,1,mem_manager_type>& project (
            const sample_type& sample 
        ) const;
        /*!
            requires
                - out_vector_size() != 0
            ensures
                - takes the given sample and projects it into the kernel feature space
                  of out_vector_size() dimensions defined by this kernel map and 
                  returns the resulting vector.
                - in more precise terms, this function returns a vector V such that:
                    - V.size() == out_vector_size()
                    - for any sample_type object S, the following equality is approximately true:
                        - get_kernel()(sample,S) == dot(V, project(S)).  
                    - The approximation error in the above equality will be zero (within rounding error)
                      if both sample_type objects involved are within the span of the set of basis 
                      samples given to the load() function.  If they are not then there will be some 
                      approximation error.  Note that all the basis samples are always within their
                      own span.  So the equality is always exact for the samples given to the load() 
                      function.
        !*/

        template <typename EXP>
        const decision_function<kernel_type> convert_to_decision_function (
            const matrix<EXP>& vect
        ) const;
        /*!
            requires
                - is_vector(vect) == true
                - vect.size() == out_vector_size()
                - out_vector_size() != 0
            ensures
                - This function interprets the given vector as a point in the kernel feature space defined 
                  by this empirical_kernel_map.  The return value of this function is a decision 
                  function, DF, that represents the given vector in the following sense:
                    - for all possible sample_type objects, S, it is the case that DF(S) == dot(project(S), vect)
                      (i.e. the returned decision function computes dot products, in kernel feature space, 
                      between vect and any argument you give it. )
                - DF.kernel_function == get_kernel()
                - DF.b == 0
                - DF.basis_vectors == these will be the basis samples given to the previous call to load().  Note
                  that it is possible for there to be fewer basis_vectors than basis samples given to load().  
        !*/

        template <typename EXP>
        const distance_function<kernel_type> convert_to_distance_function (
            const matrix<EXP>& vect
        ) const
        /*!
            requires
                - is_vector(vect) == true
                - vect.size() == out_vector_size()
                - out_vector_size() != 0
            ensures
                - This function interprets the given vector as a point in the kernel feature space defined 
                  by this empirical_kernel_map.  The return value of this function is a distance 
                  function, DF, that represents the given vector in the following sense:
                    - for all possible sample_type objects, S, it is the case that DF(S) == length(project(S) - vect)
                      (i.e. the returned distance function computes distances, in kernel feature space, 
                      between vect and any argument you give it. )
                - DF.kernel_function == get_kernel()
                - DF.b == dot(vect,vect) 
                - DF.basis_vectors == these will be the basis samples given to the previous call to load().  Note
                  that it is possible for there to be fewer basis_vectors than basis samples given to load().  
        !*/

        void swap (
            empirical_kernel_map& item
        );
        /*!
            ensures
                - swaps the state of *this and item
        !*/

    };

// ----------------------------------------------------------------------------------------

    template <
        typename kernel_type
        >
    void swap (
        empirical_kernel_map<kernel_type>& a,
        empirical_kernel_map<kernel_type>& b
    ) { a.swap(b); }
    /*!
        provides a global swap function
    !*/

    template <
        typename kernel_type
        >
    void serialize (
        const empirical_kernel_map<kernel_type>& item,
        std::ostream& out
    );
    /*!
        provides serialization support for empirical_kernel_map objects
    !*/

    template <
        typename kernel_type
        >
    void deserialize (
        empirical_kernel_map<kernel_type>& item,
        std::istream& in 
    );
    /*!
        provides serialization support for empirical_kernel_map objects
    !*/

// ----------------------------------------------------------------------------------------

}

#endif // DLIB_EMPIRICAL_KERNEl_MAP_ABSTRACT_H_

