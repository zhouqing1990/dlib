// Copyright (C) 2010  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.


#include <dlib/any.h>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>
#include "../rand.h"

#include "tester.h"


namespace  
{

    using namespace test;
    using namespace dlib;
    using namespace std;

    logger dlog("test.any");

// ----------------------------------------------------------------------------------------

    void test_contains_4(
        const any a
    )
    {
        DLIB_TEST(a.is_empty() == false);
        DLIB_TEST(a.contains<int>() == true);
        DLIB_TEST(a.contains<double>() == false);
        DLIB_TEST(any_cast<int>(a) == 4);
    }

// ----------------------------------------------------------------------------------------

    void run_test()
    {
        any a, b, c;

        DLIB_TEST(a.is_empty());
        DLIB_TEST(a.contains<int>() == false);
        DLIB_TEST(a.contains<string>() == false);
        DLIB_TEST(a.is_empty());

        a = b;

        swap(a,b);
        a.swap(b);

        a = 4;
        DLIB_TEST(a.is_empty() == false);
        DLIB_TEST(a.contains<int>() == true);
        DLIB_TEST(a.contains<double>() == false);
        DLIB_TEST(any_cast<int>(a) == 4);

        test_contains_4(a);

        DLIB_TEST(a.is_empty() == false);
        DLIB_TEST(a.contains<int>() == true);
        DLIB_TEST(a.contains<double>() == false);
        DLIB_TEST(any_cast<int>(a) == 4);

        bool error = true;
        try
        {
            any_cast<double>(a);
        }
        catch (bad_any_cast&)
        {
            error = false;
        }
        DLIB_TEST(error);

        swap(a,b);

        test_contains_4(b);

        DLIB_TEST(a.is_empty());

        a = b;

        test_contains_4(a);

        c.get<string>() = "test string";
        DLIB_TEST(c.get<string>() == "test string"); 

        a = c;
        DLIB_TEST(a.cast_to<string>() == "test string"); 


        a.clear();
        DLIB_TEST(a.is_empty());
        error = true;
        try
        {
            any_cast<string>(a);
        }
        catch (bad_any_cast&)
        {
            error = false;
        }
        DLIB_TEST(error);

    }

// ----------------------------------------------------------------------------------------

    class any_tester : public tester
    {
    public:
        any_tester (
        ) :
            tester ("test_any",
                    "Runs tests on the any component.")
        {}

        void perform_test (
        )
        {
            void run_test();
        }
    } a;

}

