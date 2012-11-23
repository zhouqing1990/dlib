// Copyright (C) 2012  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#include <dlib/svm.h>
#include <dlib/rand.h>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>

#include "tester.h"

namespace  
{

    using namespace test;
    using namespace dlib;
    using namespace std;


    logger dlog("test.ranking");

// ----------------------------------------------------------------------------------------

    template <typename T>
    void brute_force_count_ranking_inversions (
        const std::vector<T>& x,
        const std::vector<T>& y,
        std::vector<unsigned long>& x_count,
        std::vector<unsigned long>& y_count
    )
    {
        x_count.assign(x.size(),0);
        y_count.assign(y.size(),0);

        for (unsigned long i = 0; i < x.size(); ++i)
        {
            for (unsigned long j = 0; j < y.size(); ++j)
            {
                if (x[i] <= y[j])
                {
                    x_count[i]++;
                    y_count[j]++;
                }
            }
        }
    }

// ----------------------------------------------------------------------------------------

    void test_count_ranking_inversions()
    {
        print_spinner();
        dlog << LINFO << "in test_count_ranking_inversions()";

        dlib::rand rnd;
        std::vector<int> x, y;
        std::vector<unsigned long> x_count, y_count;
        std::vector<unsigned long> x_count2, y_count2;
        for (int iter = 0; iter < 5000; ++iter)
        {
            x.resize(rnd.get_random_32bit_number()%10);
            y.resize(rnd.get_random_32bit_number()%10);
            for (unsigned long i = 0; i < x.size(); ++i)
                x[i] = ((int)rnd.get_random_32bit_number()%10) - 5;
            for (unsigned long i = 0; i < y.size(); ++i)
                y[i] = ((int)rnd.get_random_32bit_number()%10) - 5;

            count_ranking_inversions(x, y, x_count, y_count);
            brute_force_count_ranking_inversions(x, y, x_count2, y_count2);

            DLIB_TEST(vector_to_matrix(x_count) == vector_to_matrix(x_count2));
            DLIB_TEST(vector_to_matrix(y_count) == vector_to_matrix(y_count2));
        }
    }

// ----------------------------------------------------------------------------------------

    void dotest1()
    {
        print_spinner();
        dlog << LINFO << "in dotest1()";

        typedef matrix<double,4,1> sample_type;

        typedef linear_kernel<sample_type> kernel_type;

        svm_rank_trainer<kernel_type> trainer;


        std::vector<ranking_pair<sample_type> > samples;

        ranking_pair<sample_type> p;
        sample_type samp;

        samp = 0, 0, 0, 1; p.relevant.push_back(samp);
        samp = 1, 0, 0, 0; p.nonrelevant.push_back(samp);
        samples.push_back(p);

        samp = 0, 0, 1, 0; p.relevant.push_back(samp);
        samp = 1, 0, 0, 0; p.nonrelevant.push_back(samp);
        samp = 0, 1, 0, 0; p.nonrelevant.push_back(samp);
        samp = 0, 1, 0, 0; p.nonrelevant.push_back(samp);
        samples.push_back(p);


        trainer.set_c(10);

        decision_function<kernel_type> df = trainer.train(samples);

        dlog << LINFO << "accuracy: "<< test_ranking_function(df, samples);
        DLIB_TEST(std::abs(test_ranking_function(df, samples) - 1.0) < 1e-14);

        DLIB_TEST(std::abs(test_ranking_function(trainer.train(samples[1]), samples) - 1.0) < 1e-14);

        trainer.set_epsilon(1e-13);
        df = trainer.train(samples);

        dlog << LINFO << df.basis_vectors(0);
        sample_type truew;
        truew = -0.5, -0.5, 0.5, 0.5;
        DLIB_TEST(length(truew - df.basis_vectors(0)) < 1e-10);

        dlog << LINFO << "accuracy: "<< test_ranking_function(df, samples);
        DLIB_TEST(std::abs(test_ranking_function(df, samples) - 1.0) < 1e-14);

        dlog << LINFO << "cv-accuracy: "<< cross_validate_ranking_trainer(trainer, samples,2);
        DLIB_TEST(std::abs(cross_validate_ranking_trainer(trainer, samples,2) - 0.7777777778) < 0.0001);

        trainer.set_learns_nonnegative_weights(true);
        df = trainer.train(samples);
        truew = 0, 0, 1.0, 1.0;
        dlog << LINFO << df.basis_vectors(0);
        DLIB_TEST(length(truew - df.basis_vectors(0)) < 1e-10);
        dlog << LINFO << "accuracy: "<< test_ranking_function(df, samples);
        DLIB_TEST(std::abs(test_ranking_function(df, samples) - 1.0) < 1e-14);


        samples.clear();
        samples.push_back(p);
        samples.push_back(p);
        samples.push_back(p);
        samples.push_back(p);
        dlog << LINFO << "cv-accuracy: "<< cross_validate_ranking_trainer(trainer, samples,4);
        DLIB_TEST(std::abs(cross_validate_ranking_trainer(trainer, samples,4) - 1) < 1e-12);
    }

// ----------------------------------------------------------------------------------------

    void dotest_sparse_vectors()
    {
        print_spinner();
        dlog << LINFO << "in dotest_sparse_vectors()";

        typedef std::map<unsigned long,double> sample_type;

        typedef sparse_linear_kernel<sample_type> kernel_type;

        svm_rank_trainer<kernel_type> trainer;


        std::vector<ranking_pair<sample_type> > samples;

        ranking_pair<sample_type> p;
        sample_type samp;

        samp[3] = 1; p.relevant.push_back(samp); samp.clear();
        samp[0] = 1; p.nonrelevant.push_back(samp); samp.clear();
        samples.push_back(p);

        samp[2] = 1; p.relevant.push_back(samp); samp.clear();
        samp[0] = 1; p.nonrelevant.push_back(samp); samp.clear();
        samp[1] = 1; p.nonrelevant.push_back(samp); samp.clear();
        samp[1] = 1; p.nonrelevant.push_back(samp); samp.clear();
        samples.push_back(p);


        trainer.set_c(10);

        decision_function<kernel_type> df = trainer.train(samples);

        dlog << LINFO << "accuracy: "<< test_ranking_function(df, samples);
        DLIB_TEST(std::abs(test_ranking_function(df, samples) - 1.0) < 1e-14);

        DLIB_TEST(std::abs(test_ranking_function(trainer.train(samples[1]), samples) - 1.0) < 1e-14);

        trainer.set_epsilon(1e-13);
        df = trainer.train(samples);

        dlog << LINFO << sparse_to_dense(df.basis_vectors(0));
        sample_type truew;
        truew[0] = -0.5;
        truew[1] = -0.5;
        truew[2] =  0.5;
        truew[3] =  0.5;
        DLIB_TEST(length(subtract(truew , df.basis_vectors(0))) < 1e-10);

        dlog << LINFO << "accuracy: "<< test_ranking_function(df, samples);
        DLIB_TEST(std::abs(test_ranking_function(df, samples) - 1.0) < 1e-14);

        dlog << LINFO << "cv-accuracy: "<< cross_validate_ranking_trainer(trainer, samples,2);
        DLIB_TEST(std::abs(cross_validate_ranking_trainer(trainer, samples,2) - 0.7777777778) < 0.0001);

        trainer.set_learns_nonnegative_weights(true);
        df = trainer.train(samples);
        truew[0] =  0.0;
        truew[1] =  0.0;
        truew[2] =  1.0;
        truew[3] =  1.0;
        dlog << LINFO << sparse_to_dense(df.basis_vectors(0));
        DLIB_TEST(length(subtract(truew , df.basis_vectors(0))) < 1e-10);
        dlog << LINFO << "accuracy: "<< test_ranking_function(df, samples);
        DLIB_TEST(std::abs(test_ranking_function(df, samples) - 1.0) < 1e-14);


        samples.clear();
        samples.push_back(p);
        samples.push_back(p);
        samples.push_back(p);
        samples.push_back(p);
        dlog << LINFO << "cv-accuracy: "<< cross_validate_ranking_trainer(trainer, samples,4);
        DLIB_TEST(std::abs(cross_validate_ranking_trainer(trainer, samples,4) - 1) < 1e-12);
    }

// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------

    class test_ranking_tools : public tester
    {
    public:
        test_ranking_tools (
        ) :
            tester ("test_ranking",
                    "Runs tests on the ranking tools.")
        {}


        void perform_test (
        )
        {
            test_count_ranking_inversions();
            dotest1();
            dotest_sparse_vectors();
        }
    } a;


}




