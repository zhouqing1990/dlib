// Copyright (C) 2006  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#ifndef DLIB_MATRIx_MAT_H__
#define DLIB_MATRIx_MAT_H__

#include "matrix_mat_abstract.h"
#include "matrix.h"
#include "../stl_checked.h"
#include <vector>
#include "matrix_op.h"
#include "../array2d.h"
#include "../array.h"


namespace dlib
{

// ----------------------------------------------------------------------------------------
    
    template <
        typename EXP
        >
    const matrix_exp<EXP>& mat (
        const matrix_exp<EXP>& m
    )
    {
        return m;
    }

// ----------------------------------------------------------------------------------------

    template <typename T>
    struct op_array2d_to_mat : does_not_alias 
    {
        op_array2d_to_mat( const T& array_) : array(array_){}

        const T& array;

        const static long cost = 1;
        const static long NR = 0;
        const static long NC = 0;
        typedef typename T::type type;
        typedef const typename T::type& const_ret_type;
        typedef typename T::mem_manager_type mem_manager_type;
        typedef row_major_layout layout_type;

        const_ret_type apply (long r, long c ) const { return array[r][c]; }

        long nr () const { return array.nr(); }
        long nc () const { return array.nc(); }
    }; 

// ----------------------------------------------------------------------------------------

    template <
        typename T,
        typename MM
        >
    const matrix_op<op_array2d_to_mat<array2d<T,MM> > > mat (
        const array2d<T,MM>& array
    )
    {
        typedef op_array2d_to_mat<array2d<T,MM> > op;
        return matrix_op<op>(op(array));
    }

// ----------------------------------------------------------------------------------------

    template <typename T>
    struct op_array_to_mat : does_not_alias 
    {
        op_array_to_mat( const T& vect_) : vect(vect_){}

        const T& vect;

        const static long cost = 1;
        const static long NR = 0;
        const static long NC = 1;
        typedef typename T::type type;
        typedef const typename T::type& const_ret_type;
        typedef typename T::mem_manager_type mem_manager_type;
        typedef row_major_layout layout_type;

        const_ret_type apply (long r, long  ) const { return vect[r]; }

        long nr () const { return vect.size(); }
        long nc () const { return 1; }
    }; 

// ----------------------------------------------------------------------------------------

    template <
        typename T,
        typename MM
        >
    const matrix_op<op_array_to_mat<array<T,MM> > > mat (
        const array<T,MM>& m 
    )
    {
        typedef op_array_to_mat<array<T,MM> > op;
        return matrix_op<op>(op(m));
    }

// ----------------------------------------------------------------------------------------

    template <typename T>
    struct op_std_vect_to_mat : does_not_alias 
    {
        op_std_vect_to_mat( const T& vect_) : vect(vect_){}

        const T& vect;

        const static long cost = 1;
        const static long NR = 0;
        const static long NC = 1;
        typedef typename T::value_type type;
        typedef const typename T::value_type& const_ret_type;
        typedef default_memory_manager mem_manager_type;
        typedef row_major_layout layout_type;

        const_ret_type apply (long r, long ) const { return vect[r]; }

        long nr () const { return vect.size(); }
        long nc () const { return 1; }
    }; 

// ----------------------------------------------------------------------------------------

    template <
        typename value_type,
        typename alloc
        >
    const matrix_op<op_std_vect_to_mat<std::vector<value_type,alloc> > > mat (
        const std::vector<value_type,alloc>& vector
    )
    {
        typedef op_std_vect_to_mat<std::vector<value_type,alloc> > op;
        return matrix_op<op>(op(vector));
    }

// ----------------------------------------------------------------------------------------

    template <
        typename value_type,
        typename alloc
        >
    const matrix_op<op_std_vect_to_mat<std::vector<value_type,alloc> > > mat (
        const std_vector_c<value_type,alloc>& vector
    )
    {
        typedef op_std_vect_to_mat<std_vector_c<value_type,alloc> > op;
        return matrix_op<op>(op(vector));
    }

// ----------------------------------------------------------------------------------------

    template <typename T>
    struct op_pointer_to_col_vect : does_not_alias 
    {
        op_pointer_to_col_vect(
            const T* ptr_,
            const long size_
        ) : ptr(ptr_), size(size_){}

        const T* ptr;
        const long size;

        const static long cost = 1;
        const static long NR = 0;
        const static long NC = 1;
        typedef T type;
        typedef const T& const_ret_type;
        typedef default_memory_manager mem_manager_type;
        typedef row_major_layout layout_type;

        const_ret_type apply (long r, long ) const { return ptr[r]; }

        long nr () const { return size; }
        long nc () const { return 1; }
    }; 

// ----------------------------------------------------------------------------------------

    template <
        typename T
        >
    const matrix_op<op_pointer_to_col_vect<T> > mat (
        const T* ptr,
        long nr
    )
    {
        DLIB_ASSERT(nr > 0 , 
                    "\tconst matrix_exp mat(ptr, nr)"
                    << "\n\t nr must be bigger than 0"
                    << "\n\t nr: " << nr
        );
        typedef op_pointer_to_col_vect<T> op;
        return matrix_op<op>(op(ptr, nr));
    }

// ----------------------------------------------------------------------------------------

    template <typename T>
    struct op_pointer_to_mat : does_not_alias 
    {
        op_pointer_to_mat(
            const T* ptr_,
            const long nr_,
            const long nc_ 
        ) : ptr(ptr_), rows(nr_), cols(nc_){}

        const T* ptr;
        const long rows;
        const long cols;

        const static long cost = 1;
        const static long NR = 0;
        const static long NC = 0;
        typedef T type;
        typedef const T& const_ret_type;
        typedef default_memory_manager mem_manager_type;
        typedef row_major_layout layout_type;

        const_ret_type apply (long r, long c) const { return ptr[r*cols + c]; }

        long nr () const { return rows; }
        long nc () const { return cols; }
    }; 

// ----------------------------------------------------------------------------------------

    template <
        typename T
        >
    const matrix_op<op_pointer_to_mat<T> > mat (
        const T* ptr,
        long nr,
        long nc
    )
    {
        DLIB_ASSERT(nr > 0 && nc > 0 , 
                    "\tconst matrix_exp mat(ptr, nr, nc)"
                    << "\n\t nr and nc must be bigger than 0"
                    << "\n\t nr: " << nr
                    << "\n\t nc: " << nc
        );
        typedef op_pointer_to_mat<T> op;
        return matrix_op<op>(op(ptr,nr,nc));
    }

// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------
//                                  DEPRECATED FUNCTIONS
// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------

// vector_to_matrix(), array_to_matrix(), pointer_to_matrix(), and
// pointer_to_column_vector() have been deprecated in favor of the more uniform mat()
// function.  But they are here for backwards compatibility.

    template <
        typename vector_type
        >
    const typename disable_if<is_matrix<vector_type>, matrix_op<op_array_to_mat<vector_type> > >::type 
    vector_to_matrix (
        const vector_type& vector
    )
    {
        typedef op_array_to_mat<vector_type> op;
        return matrix_op<op>(op(vector));
    }

    template <
        typename vector_type
        >
    const typename enable_if<is_matrix<vector_type>,vector_type>::type& vector_to_matrix (
        const vector_type& vector
    )
    /*!
        This overload catches the case where the argument to this function is
        already a matrix.
    !*/
    {
        return vector;
    }

    template <
        typename value_type,
        typename alloc
        >
    const matrix_op<op_std_vect_to_mat<std::vector<value_type,alloc> > > vector_to_matrix (
        const std::vector<value_type,alloc>& vector
    )
    {
        typedef op_std_vect_to_mat<std::vector<value_type,alloc> > op;
        return matrix_op<op>(op(vector));
    }

    template <
        typename value_type,
        typename alloc
        >
    const matrix_op<op_std_vect_to_mat<std_vector_c<value_type,alloc> > > vector_to_matrix (
        const std_vector_c<value_type,alloc>& vector
    )
    {
        typedef op_std_vect_to_mat<std_vector_c<value_type,alloc> > op;
        return matrix_op<op>(op(vector));
    }

// ----------------------------------------------------------------------------------------

    template <
        typename array_type
        >
    const typename enable_if<is_matrix<array_type>,array_type>::type& 
    array_to_matrix (
        const array_type& array
    )
    {
        return array;
    }

    template <
        typename array_type
        >
    const typename disable_if<is_matrix<array_type>,matrix_op<op_array2d_to_mat<array_type> > >::type 
    array_to_matrix (
        const array_type& array
    )
    {
        typedef op_array2d_to_mat<array_type> op;
        return matrix_op<op>(op(array));
    }

// ----------------------------------------------------------------------------------------

    template <
        typename T
        >
    const matrix_op<op_pointer_to_mat<T> > pointer_to_matrix (
        const T* ptr,
        long nr,
        long nc
    )
    {
        DLIB_ASSERT(nr > 0 && nc > 0 , 
                    "\tconst matrix_exp pointer_to_matrix(ptr, nr, nc)"
                    << "\n\t nr and nc must be bigger than 0"
                    << "\n\t nr: " << nr
                    << "\n\t nc: " << nc
        );
        typedef op_pointer_to_mat<T> op;
        return matrix_op<op>(op(ptr,nr,nc));
    }

    template <
        typename T
        >
    const matrix_op<op_pointer_to_col_vect<T> > pointer_to_column_vector (
        const T* ptr,
        long nr
    )
    {
        DLIB_ASSERT(nr > 0 , 
                    "\tconst matrix_exp pointer_to_column_vector(ptr, nr)"
                    << "\n\t nr must be bigger than 0"
                    << "\n\t nr: " << nr
        );
        typedef op_pointer_to_col_vect<T> op;
        return matrix_op<op>(op(ptr, nr));
    }

// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------

}

#endif // DLIB_MATRIx_MAT_H__


