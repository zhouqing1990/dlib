// Copyright (C) 2003  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#undef DLIB_POINT_TrANSFORMS_ABSTRACT_H__
#ifdef DLIB_POINT_TrANSFORMS_ABSTRACT_H__

#include "../matrix/matrix_abstract.h"
#include "vector_abstract.h"

namespace dlib
{

// ----------------------------------------------------------------------------------------

    class point_transform_affine
    {
        /*!
            WHAT THIS OBJECT REPRESENTS
                This is an object that takes 2D points or vectors and 
                applies an affine transformation to them.
        !*/
    public:
        point_transform_affine (
            const matrix<double,2,2>& m,
            const dlib::vector<double,2>& b
        );
        /*!
            ensures
                - When (*this)(p) is invoked it will return a point P such that:
                    - P == m*p + b
        !*/

        const dlib::vector<double,2> operator() (
            const dlib::vector<double,2>& p
        ) const;
        /*!
            ensures
                - applies the affine transformation defined by this object's constructor
                  to p and returns the result.
        !*/

    };

// ----------------------------------------------------------------------------------------

    class point_transform
    {
        /*!
            WHAT THIS OBJECT REPRESENTS
                This is an object that takes 2D points or vectors and 
                rotates them around the origin by a given angle and then
                translates them.
        !*/
    public:
        point_transform (
            const double& angle,
            const dlib::vector<double,2>& translate
        )
        /*!
            ensures
                - When (*this)(p) is invoked it will return a point P such that:
                    - P is the point p rotated counter-clockwise around the origin 
                      angle radians and then shifted by having translate added to it.
                      (Note that this is counter clockwise with respect to the normal
                      coordinate system with positive y going up and positive x going
                      to the right)
        !*/

        template <typename T>
        const dlib::vector<T,2> operator() (
            const dlib::vector<T,2>& p
        ) const;
        /*!
            ensures
                - rotates p, then translates it and returns the result
        !*/
    };

// ----------------------------------------------------------------------------------------

    class point_rotator
    {
        /*!
            WHAT THIS OBJECT REPRESENTS
                This is an object that takes 2D points or vectors and 
                rotates them around the origin by a given angle.
        !*/
    public:
        point_rotator (
            const double& angle
        );
        /*!
            ensures
                - When (*this)(p) is invoked it will return a point P such that:
                    - P is the point p rotated counter-clockwise around the origin 
                      angle radians.
                      (Note that this is counter clockwise with respect to the normal
                      coordinate system with positive y going up and positive x going
                      to the right)
        !*/

        template <typename T>
        const dlib::vector<T,2> operator() (
            const dlib::vector<T,2>& p
        ) const;
        /*!
            ensures
                - rotates p and returns the result
        !*/
    };

// ----------------------------------------------------------------------------------------

    template <typename T>
    const dlib::vector<T,2> rotate_point (
        const dlib::vector<T,2> center,
        const dlib::vector<T,2> p,
        double angle
    );
    /*!
        ensures
            - returns a point P such that:
                - P is the point p rotated counter-clockwise around the given
                  center point by angle radians.
                  (Note that this is counter clockwise with respect to the normal
                  coordinate system with positive y going up and positive x going
                  to the right)
    !*/

// ----------------------------------------------------------------------------------------

    matrix<double,2,2> rotation_matrix (
         double angle
    );
    /*!
        ensures
            - returns a rotation matrix which rotates points around the origin in a
              counter-clockwise direction by angle radians.
              (Note that this is counter clockwise with respect to the normal
              coordinate system with positive y going up and positive x going
              to the right)
              Or in other words, this function returns a matrix M such that, given a
              point P, M*P gives a point which is P rotated by angle radians around
              the origin in a counter-clockwise direction.
    !*/

// ----------------------------------------------------------------------------------------

}

#endif // DLIB_POINT_TrANSFORMS_ABSTRACT_H__


