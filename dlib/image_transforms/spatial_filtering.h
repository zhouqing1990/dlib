// Copyright (C) 2006  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#ifndef DLIB_SPATIAL_FILTERINg_H_
#define DLIB_SPATIAL_FILTERINg_H_

#include "../pixel.h"
#include "spatial_filtering_abstract.h"
#include "../algs.h"
#include "../assert.h"
#include "../array2d.h"
#include "../matrix.h"
#include <limits>

namespace dlib
{

// ----------------------------------------------------------------------------------------

    template <
        typename in_image_type,
        typename out_image_type,
        typename EXP,
        typename T
        >
    void spatially_filter_image (
        const in_image_type& in_img,
        out_image_type& out_img,
        const matrix_exp<EXP>& filter,
        T scale,
        bool use_abs = false,
        bool add_to = false
    )
    {
        COMPILE_TIME_ASSERT( pixel_traits<typename in_image_type::type>::has_alpha == false );
        COMPILE_TIME_ASSERT( pixel_traits<typename out_image_type::type>::has_alpha == false );

        DLIB_ASSERT(scale != 0 &&
                    filter.nr()%2 == 1 &&
                    filter.nc()%2 == 1,
            "\tvoid spatially_filter_image()"
            << "\n\t You can't give a scale of zero or a filter with even dimensions"
            << "\n\t scale: "<< scale
            << "\n\t filter.nr(): "<< filter.nr()
            << "\n\t filter.nc(): "<< filter.nc()
            );
        DLIB_ASSERT(is_same_object(in_img, out_img) == false,
            "\tvoid spatially_filter_image()"
            << "\n\tYou must give two different image objects"
            );



        // if there isn't any input image then don't do anything
        if (in_img.size() == 0)
        {
            out_img.clear();
            return;
        }

        out_img.set_size(in_img.nr(),in_img.nc());

        zero_border_pixels(out_img, filter.nc()/2, filter.nr()/2); 

        // figure out the range that we should apply the filter to
        const long first_row = filter.nr()/2;
        const long first_col = filter.nc()/2;
        const long last_row = in_img.nr() - filter.nr()/2;
        const long last_col = in_img.nc() - filter.nc()/2;

        // apply the filter to the image
        for (long r = first_row; r < last_row; ++r)
        {
            for (long c = first_col; c < last_col; ++c)
            {
                typedef typename EXP::type ptype;
                ptype p;
                ptype temp = 0;
                for (long m = 0; m < filter.nr(); ++m)
                {
                    for (long n = 0; n < filter.nc(); ++n)
                    {
                        // pull out the current pixel and put it into p
                        p = get_pixel_intensity(in_img[r-filter.nr()/2+m][c-filter.nc()/2+n]);
                        temp += p*filter(m,n);
                    }
                }

                temp /= scale;

                if (use_abs && temp < 0)
                {
                    temp = -temp;
                }

                // save this pixel to the output image
                if (add_to == false)
                {
                    assign_pixel(out_img[r][c], in_img[r][c]);
                    assign_pixel_intensity(out_img[r][c], temp);
                }
                else
                {
                    assign_pixel(out_img[r][c], temp + get_pixel_intensity(out_img[r][c]));
                }
            }
        }
    }

    template <
        typename in_image_type,
        typename out_image_type,
        typename EXP
        >
    void spatially_filter_image (
        const in_image_type& in_img,
        out_image_type& out_img,
        const matrix_exp<EXP>& filter
    )
    {
        spatially_filter_image(in_img,out_img,filter,1);
    }

// ----------------------------------------------------------------------------------------

    template <
        typename in_image_type,
        typename out_image_type,
        typename EXP1,
        typename EXP2,
        typename T
        >
    void spatially_filter_image_separable (
        const in_image_type& in_img,
        out_image_type& out_img,
        const matrix_exp<EXP1>& row_filter,
        const matrix_exp<EXP2>& col_filter,
        T scale,
        bool use_abs = false,
        bool add_to = false
    )
    {
        COMPILE_TIME_ASSERT( pixel_traits<typename in_image_type::type>::has_alpha == false );
        COMPILE_TIME_ASSERT( pixel_traits<typename out_image_type::type>::has_alpha == false );

        DLIB_ASSERT(scale != 0 &&
                    row_filter.size()%2 == 1 &&
                    col_filter.size()%2 == 1 &&
                    is_vector(row_filter) &&
                    is_vector(col_filter),
            "\tvoid spatially_filter_image_separable()"
            << "\n\t Invalid inputs were given to this function."
            << "\n\t scale: "<< scale
            << "\n\t row_filter.size(): "<< row_filter.size()
            << "\n\t col_filter.size(): "<< col_filter.size()
            << "\n\t is_vector(row_filter): "<< is_vector(row_filter)
            << "\n\t is_vector(col_filter): "<< is_vector(col_filter)
            );
        DLIB_ASSERT(is_same_object(in_img, out_img) == false,
            "\tvoid spatially_filter_image_separable()"
            << "\n\tYou must give two different image objects"
            );



        // if there isn't any input image then don't do anything
        if (in_img.size() == 0)
        {
            out_img.clear();
            return;
        }

        out_img.set_size(in_img.nr(),in_img.nc());

        zero_border_pixels(out_img, row_filter.size()/2, col_filter.size()/2); 

        // figure out the range that we should apply the filter to
        const long first_row = col_filter.size()/2;
        const long first_col = row_filter.size()/2;
        const long last_row = in_img.nr() - col_filter.size()/2;
        const long last_col = in_img.nc() - row_filter.size()/2;


        typedef typename out_image_type::mem_manager_type mem_manager_type;
        typedef typename EXP1::type ptype;

        array2d<ptype,mem_manager_type> temp_img;
        temp_img.set_size(in_img.nr(), in_img.nc());

        // apply the row filter
        for (long r = 0; r < in_img.nr(); ++r)
        {
            for (long c = first_col; c < last_col; ++c)
            {
                ptype p;
                ptype temp = 0;
                for (long n = 0; n < row_filter.size(); ++n)
                {
                    // pull out the current pixel and put it into p
                    p = get_pixel_intensity(in_img[r][c-row_filter.size()/2+n]);
                    temp += p*row_filter(n);
                }
                temp_img[r][c] = temp;
            }
        }

        // apply the column filter 
        for (long r = first_row; r < last_row; ++r)
        {
            for (long c = first_col; c < last_col; ++c)
            {
                ptype temp = 0;
                for (long m = 0; m < col_filter.size(); ++m)
                {
                    temp += temp_img[r-col_filter.size()/2+m][c]*col_filter(m);
                }

                temp /= scale;

                if (use_abs && temp < 0)
                {
                    temp = -temp;
                }

                // save this pixel to the output image
                if (add_to == false)
                {
                    assign_pixel(out_img[r][c], in_img[r][c]);
                    assign_pixel_intensity(out_img[r][c], temp);
                }
                else
                {
                    assign_pixel(out_img[r][c], temp + get_pixel_intensity(out_img[r][c]));
                }
            }
        }
    }

    template <
        typename in_image_type,
        typename out_image_type,
        typename EXP1,
        typename EXP2
        >
    void spatially_filter_image_separable (
        const in_image_type& in_img,
        out_image_type& out_img,
        const matrix_exp<EXP1>& row_filter,
        const matrix_exp<EXP2>& col_filter
    )
    {
        spatially_filter_image_separable(in_img,out_img,row_filter,col_filter,1);
    }

// ----------------------------------------------------------------------------------------

    template <
        long NR,
        long NC,
        typename T,
        typename in_image_type
        >
    inline void separable_3x3_filter_block_grayscale (
        T (&block)[NR][NC],
        const in_image_type& img,
        const long& r,
        const long& c,
        const T& fe1, // separable filter end
        const T& fm,  // separable filter middle 
        const T& fe2 // separable filter end 2
    ) 
    {
        // make sure requires clause is not broken
        DLIB_ASSERT(shrink_rect(get_rect(img),1).contains(c,r) &&
                    shrink_rect(get_rect(img),1).contains(c+NC-1,r+NR-1),
            "\t void separable_3x3_filter_block_grayscale()"
            << "\n\t The sub-window doesn't fit inside the given image."
            << "\n\t get_rect(img):       " << get_rect(img) 
            << "\n\t (c,r):               " << point(c,r) 
            << "\n\t (c+NC-1,r+NR-1): " << point(c+NC-1,r+NR-1) 
            );


        T row_filt[NR+2][NC];
        for (long rr = 0; rr < NR+2; ++rr)
        {
            for (long cc = 0; cc < NC; ++cc)
            {
                row_filt[rr][cc] = get_pixel_intensity(img[r+rr-1][c+cc-1])*fe1 + 
                                   get_pixel_intensity(img[r+rr-1][c+cc])*fm + 
                                   get_pixel_intensity(img[r+rr-1][c+cc+1])*fe2;
            }
        }

        for (long rr = 0; rr < NR; ++rr)
        {
            for (long cc = 0; cc < NC; ++cc)
            {
                block[rr][cc] = (row_filt[rr][cc]*fe1 + 
                                row_filt[rr+1][cc]*fm + 
                                row_filt[rr+2][cc]*fe2);
            }
        }

    }

// ----------------------------------------------------------------------------------------

    template <
        long NR,
        long NC,
        typename T,
        typename U,
        typename in_image_type
        >
    inline void separable_3x3_filter_block_rgb (
        T (&block)[NR][NC],
        const in_image_type& img,
        const long& r,
        const long& c,
        const U& fe1, // separable filter end
        const U& fm,  // separable filter middle 
        const U& fe2  // separable filter end 2
    ) 
    {
        // make sure requires clause is not broken
        DLIB_ASSERT(shrink_rect(get_rect(img),1).contains(c,r) &&
                    shrink_rect(get_rect(img),1).contains(c+NC-1,r+NR-1),
            "\t void separable_3x3_filter_block_grayscale()"
            << "\n\t The sub-window doesn't fit inside the given image."
            << "\n\t get_rect(img):       " << get_rect(img) 
            << "\n\t (c,r):               " << point(c,r) 
            << "\n\t (c+NC-1,r+NR-1): " << point(c+NC-1,r+NR-1) 
            );

        T row_filt[NR+2][NC];
        for (long rr = 0; rr < NR+2; ++rr)
        {
            for (long cc = 0; cc < NC; ++cc)
            {
                row_filt[rr][cc].red   = img[r+rr-1][c+cc-1].red*fe1   + img[r+rr-1][c+cc].red*fm   + img[r+rr-1][c+cc+1].red*fe2;
                row_filt[rr][cc].green = img[r+rr-1][c+cc-1].green*fe1 + img[r+rr-1][c+cc].green*fm + img[r+rr-1][c+cc+1].green*fe2;
                row_filt[rr][cc].blue  = img[r+rr-1][c+cc-1].blue*fe1  + img[r+rr-1][c+cc].blue*fm  + img[r+rr-1][c+cc+1].blue*fe2;
            }
        }

        for (long rr = 0; rr < NR; ++rr)
        {
            for (long cc = 0; cc < NC; ++cc)
            {
                block[rr][cc].red   = row_filt[rr][cc].red*fe1   + row_filt[rr+1][cc].red*fm   + row_filt[rr+2][cc].red*fe2;
                block[rr][cc].green = row_filt[rr][cc].green*fe1 + row_filt[rr+1][cc].green*fm + row_filt[rr+2][cc].green*fe2;
                block[rr][cc].blue  = row_filt[rr][cc].blue*fe1  + row_filt[rr+1][cc].blue*fm  + row_filt[rr+2][cc].blue*fe2;
            }
        }

    }

// ----------------------------------------------------------------------------------------

    inline double gaussian (
        double x, 
        double sigma
    )
    /*!
        requires
            - sigma > 0
        ensures
            - computes and returns the value of a 1D Gaussian function with mean 0 
              and standard deviation sigma at the given x value.
    !*/
    {
        DLIB_ASSERT(sigma > 0,
            "\tdouble gaussian(x)"
            << "\n\t sigma must be bigger than 0"
            << "\n\t sigma: " << sigma 
        );
        const double sqrt_2_pi = 2.5066282746310002416123552393401041626930;
        return 1.0/(sigma*sqrt_2_pi) * std::exp( -(x*x)/(2*sigma*sigma));
    }

// ----------------------------------------------------------------------------------------

    template <
        typename T
        >
    matrix<T,0,1> create_gaussian_filter (
        double sigma,
        int max_size 
    )
    /*!
        requires
            - sigma > 0
            - max_size > 0 
            - max_size is an odd number
        ensures
            - returns a separable Gaussian filter F such that:
                - is_vector(F) == true 
                - F.size() <= max_size 
                - F is suitable for use with the spatially_filter_image_separable() routine
                  and its use with this function corresponds to running a Gaussian filter 
                  of sigma width over an image.
    !*/
    {
        DLIB_ASSERT(sigma > 0 && max_size > 0 && (max_size%2)==1,
            "\t matrix<T,0,1> create_gaussian_filter()"
            << "\n\t Invalid inputs were given to this function."
            << "\n\t sigma: " << sigma 
            << "\n\t max_size:  " << max_size 
        );

        // Adjust the size so that the ratio of the gaussian values isn't huge.  
        // This only matters when T is an integer type.  However, we do it for
        // all types so that the behavior of this function is always relatively
        // the same.
        while (gaussian(0,sigma)/gaussian(max_size/2,sigma) > 50)
            --max_size;


        matrix<double,0,1> f(max_size);
        for (long i = 0; i < f.size(); ++i)
        {
            f(i) = gaussian(i-max_size/2, sigma);
        }

        if (is_float_type<T>::value == false)
        {
            f /= f(0);
            return matrix_cast<T>(round(f));
        }
        else
        {
            return matrix_cast<T>(f);
        }
    }

// ----------------------------------------------------------------------------------------

    template <
        typename in_image_type,
        typename out_image_type
        >
    void gaussian_blur (
        const in_image_type& in_img,
        out_image_type& out_img,
        double sigma = 1,
        int max_size = 1001
    )
    {
        DLIB_ASSERT(sigma > 0 && max_size > 0 && (max_size%2)==1 &&
                    is_same_object(in_img, out_img) == false,
            "\t void gaussian_blur()"
            << "\n\t Invalid inputs were given to this function."
            << "\n\t sigma: " << sigma 
            << "\n\t max_size:  " << max_size 
            << "\n\t is_same_object(in_img,out_img): " << is_same_object(in_img,out_img) 
        );

        typedef typename pixel_traits<typename out_image_type::type>::basic_pixel_type type;
        typedef typename promote<type>::type ptype;

        const matrix<ptype,0,1>& filt = create_gaussian_filter<ptype>(sigma, max_size);

        ptype scale = sum(filt);
        scale = scale*scale;

        spatially_filter_image_separable(in_img, out_img, filt, filt, scale);

    }

// ----------------------------------------------------------------------------------------

}

#endif // DLIB_SPATIAL_FILTERINg_H_


