// Copyright (C) 2013 Steve Taylor (steve98654@gmail.com)
// License: Boost Software License  See LICENSE.txt for full license

#ifndef DLIB_INTEGRATE_FUNCTION_ADAPT_SIMPSON__
#define DLIB_INTEGRATE_FUNCTION_ADAPT_SIMPSON__

#include "../matrix.h"

class adapt_simp
{
public:

template <typename T, typename funct>
T integrate_function_adapt_simp(const funct& f, T a, T b, T tol)
{
    T eps = std::numeric_limits<double>::epsilon();

    if(tol < eps)
    {
        tol = eps;
    }

    const T ba = b-a;
    const T fa = f(a);
    const T fb = f(b);
    const T fm = f((a+b)/2);

    T is =ba/8*(fa+fb+fm+ f(a + 0.9501*ba) + f(a + 0.2311*ba) + f(a + 0.6068*ba)
                           + f(a + 0.4860*ba) + f(a + 0.8913*ba));

    if(is == 0)
    {
        is = b-a;
    }

    is = is*tol/eps;

    int cnt = 0;

    T tstvl = adapt_simp_stop(f, a, b, fa, fm, fb, is, cnt);

    return tstvl;

}

private: 

template <typename T, typename funct>
T adapt_simp_stop(const funct& f, T a, T b, T fa, T fm, T fb, T is, int &cnt)
{
    int MAXINT = 1000;
   
    T m   = (a + b)/2.0;
    T h   = (b - a)/4.0;
    T fml = f(a + h);
    T fmr = f(b - h);
    T i1 = h/1.5*(fa+4.0*fm+fb);
    T i2 = h/3.0*(fa+4.0*(fml+fmr)+2.0*fm+fb);
    i1 = (16.0*i2 - i1)/15.0;
    T Q = 0;

    if((is+(i1-i2) == is) || (m <= a) || (b <= m))
    {
        if((m <= a) || (b <= m))
        {
            DLIB_ASSERT(1, "\tintegrate_function_adapt_simpson::adapt_simp_stop"
                           << "\n\tmidpoint evaluation occurred at endpoint");
        }
    
        Q = i1;
    }
    else 
    {
        if(cnt < MAXINT)
        {cnt = cnt + 1;

            Q = adapt_simp_stop(f,a,m,fa,fml,fm,is,cnt) 
              + adapt_simp_stop(f,m,b,fm,fmr,fb,is,cnt); 
        }
    }

    return Q;
}

};

#endif //DLIB_INTEGRATE_FUNCTION_ADAPT_SIMPSON.h__

