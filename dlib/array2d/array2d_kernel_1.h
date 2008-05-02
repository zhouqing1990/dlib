// Copyright (C) 2006  Davis E. King (davisking@users.sourceforge.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#ifndef DLIB_ARRAY2D_KERNEl_1_
#define DLIB_ARRAY2D_KERNEl_1_

#include "array2d_kernel_abstract.h"
#include "../algs.h"
#include "../interfaces/enumerable.h"
#include "../serialize.h"
#include "../memory_manager.h"

namespace dlib
{
    template <
        typename T,
        typename mem_manager = memory_manager<char>::kernel_1a
        >
    class array2d_kernel_1 : public enumerable<T>
    {

        /*!
            INITIAL VALUE
                - nc_ == 0 
                - nr_ == 0 
                - data == 0 
                - rows == 0
                - at_start_ == true
                - cur == 0
                - last == 0

            CONVENTION
                - nc_ == nc() 
                - nr_ == nc() 
                - if (data != 0) then
                    - last == a pointer to the last element in the data array
                    - data == pointer to an array of nc_*nr_ T objects 
                    - rows == pointer to an array of nr_ row objects
                    - for all x < nr_:
                        - rows[x].data == data + x*nc_
                        - rows[x].nc_ == nc_
                - else
                    - nc_ == 0
                    - nr_ == 0
                    - data == 0
                    - rows == 0
                    - last == 0


                - nr_ * nc_ == size()
                - if (cur == 0) then
                    - current_element_valid() == false
                - else 
                    - current_element_valid() == true
                    - *cur == element()

                - at_start_ == at_start()      
        !*/


        class row_helper;
    public:
         
        typedef T type;
        typedef mem_manager mem_manager_type;

        // -----------------------------------

        class row 
        {
            /*!
                CONVENTION
                    - nc_ == nc()
                    - for all x < nc_:
                        - (*this)[x] == data[x]
            !*/

            friend class array2d_kernel_1;
            friend class row_helper;

        public:
            long nc (
            ) const { return nc_; }

            const T& operator[] (
                long column
            ) const { return data[column]; }

            T& operator[] (
                long column
            ) { return data[column]; }

        private:

            long nc_;
            T* data; 


            // restricted functions
            row(){}
            row(row&);
            row& operator=(row&);
        };

        // -----------------------------------

        array2d_kernel_1 (
        ) : 
            nc_(0),
            nr_(0),
            rows(0),
            data(0),
            cur(0),
            last(0),
            at_start_(true)
        {
        }

        virtual ~array2d_kernel_1 (
        ) { clear(); }

        long nc (
        ) const { return nc_; }

        long nr (
        ) const { return nr_; }

        row& operator[] (
            long row
        ) { return rows[row]; }

        const row& operator[] (
            long row
        ) const { return rows[row]; }

        void swap (
            array2d_kernel_1& item
        )
        {
            exchange(data,item.data);
            exchange(rows,item.rows);
            exchange(nr_,item.nr_);
            exchange(nc_,item.nc_);
            exchange(at_start_,item.at_start_);
            exchange(cur,item.cur);
            exchange(last,item.last);
            pool.swap(item.pool);
            rpool.swap(item.rpool);
        }

        void clear (
        )
        {
            if (data != 0)
            {
                rpool.deallocate_array(reinterpret_cast<row_helper*>(rows));
                pool.deallocate_array(data);
                nc_ = 0;
                nr_ = 0;
                rows = 0;
                data = 0;
                at_start_ = true;
                cur = 0;
                last = 0;
            }
        }

        void set_size (
            long nr__,
            long nc__
        );

        bool at_start (
        ) const { return at_start_; }

        void reset (
        ) const { at_start_ = true; cur = 0; }

        bool current_element_valid (
        ) const { return (cur != 0); }

        const T& element (
        ) const { return *cur; }

        T& element (
        ) { return *cur; }

        bool move_next (
        ) const
        {
            if (cur != 0)
            {
                if (cur != last)
                {
                    ++cur;
                    return true;
                }
                cur = 0;
                return false;
            }
            else if (at_start_)
            {
                cur = data;
                at_start_ = false;
                return (data != 0);
            }
            else
            {
                return false;
            }
        }

        unsigned long size (
        ) const { return static_cast<unsigned long>(nc_ * nr_); }

    private:

        // this object exists just so we can have a row type object that
        // has a public default constructor so the memory_manager can construct it.
        // I would have made rpool a friend of row but some compilers can't handle 
        // that without crapping out.
        class row_helper : public row {};

        typename mem_manager::template rebind<T>::other pool;
        typename mem_manager::template rebind<row_helper>::other rpool;

        long nc_;
        long nr_;
        row* rows;
        T* data;

        mutable T* cur;
        T* last;
        mutable bool at_start_;

        // restricted functions
        array2d_kernel_1(array2d_kernel_1&);        // copy constructor
        array2d_kernel_1& operator=(array2d_kernel_1&);    // assignment operator

    };

// ----------------------------------------------------------------------------------------

    template <
        typename T,
        typename mem_manager
        >
    inline void swap (
        array2d_kernel_1<T,mem_manager>& a, 
        array2d_kernel_1<T,mem_manager>& b 
    ) { a.swap(b); }   


    template <
        typename T,
        typename mem_manager
        >
    void serialize (
        const array2d_kernel_1<T,mem_manager>& item, 
        std::ostream& out 
    )   
    {
        try
        {
            serialize(item.nc(),out);
            serialize(item.nr(),out);

            item.reset();
            while (item.move_next())
                serialize(item.element(),out);
            item.reset();
        }
        catch (serialization_error e)
        { 
            throw serialization_error(e.info + "\n   while serializing object of type array2d_kernel_1"); 
        }
    }

    template <
        typename T 
        >
    void deserialize (
        array2d_kernel_1<T>& item, 
        std::istream& in
    )   
    {
        try
        {
            long nc, nr;
            deserialize(nc,in);
            deserialize(nr,in);

            item.set_size(nr,nc);

            while (item.move_next())
                deserialize(item.element(),in); 
            item.reset();
        }
        catch (serialization_error e)
        { 
            item.clear();
            throw serialization_error(e.info + "\n   while deserializing object of type array2d_kernel_1"); 
        }
    }

// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------
    // member function definitions
// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------

    template <
        typename T,
        typename mem_manager
        >
    void array2d_kernel_1<T,mem_manager>::
    set_size (
        long nr__,
        long nc__
    )
    {
        // set the enumerator back at the start
        at_start_ = true;
        cur = 0;

        // don't do anything if we are already the right size.
        if (nc_ == nc__ && nr_ == nr__)
        {
            return;
        }

        nc_ = nc__;
        nr_ = nr__;

        // free any existing memory
        if (data != 0)
        {
            pool.deallocate_array(data);
            rpool.deallocate_array(reinterpret_cast<row_helper*>(rows));
            data = 0;
            rows = 0;
        }

        // now setup this object to have the new size
        try
        {
            if (nr_ > 0)
            {
                rows = rpool.allocate_array(nr_);
                data = pool.allocate_array(nr_*nc_);
                last = data + nr_*nc_ - 1;
            }
        }
        catch (...)
        {
            if (rows)
                rpool.deallocate_array(reinterpret_cast<row_helper*>(rows));
            if (data)
                pool.deallocate_array(data);

            rows = 0;
            data = 0;
            nc_ = 0;
            nr_ = 0;
            last = 0;
            throw;
        }

        // now set up all the rows
        for (long i = 0; i < nr_; ++i)
        {
            rows[i].nc_ = nc_;
            rows[i].data = data + i*nc_;
        }
    }

// ----------------------------------------------------------------------------------------

}

#endif // DLIB_ARRAY2D_KERNEl_1_ 

