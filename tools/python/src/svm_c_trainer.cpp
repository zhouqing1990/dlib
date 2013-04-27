
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>
#include <dlib/matrix.h>
#include "serialize_pickle.h"
#include <dlib/svm.h>
#include "pyassert.h"

using namespace dlib;
using namespace std;
using namespace boost::python;

typedef matrix<double,0,1> sample_type; 
typedef std::vector<std::pair<unsigned long,double> > sparse_vect;

template <typename trainer_type>
typename trainer_type::trained_function_type train (
    const trainer_type& trainer,
    const std::vector<typename trainer_type::sample_type>& samples,
    const std::vector<double>& labels
)
{
    pyassert(is_binary_classification_problem(samples,labels), "Invalid inputs");
    return trainer.train(samples, labels);
}

template <typename trainer_type>
void set_epsilon ( trainer_type& trainer, double eps)
{
    pyassert(eps > 0, "epsilon must be > 0");
    trainer.set_epsilon(eps);
}

template <typename trainer_type>
double get_epsilon ( const trainer_type& trainer) { return trainer.get_epsilon(); }


template <typename trainer_type>
void set_cache_size ( trainer_type& trainer, long cache_size)
{
    pyassert(cache_size > 0, "cache size must be > 0");
    trainer.set_cache_size(cache_size);
}

template <typename trainer_type>
long get_cache_size ( const trainer_type& trainer) { return trainer.get_cache_size(); }


template <typename trainer_type>
void set_c ( trainer_type& trainer, double C)
{
    pyassert(C > 0, "C must be > 0");
    trainer.set_c(C);
}

template <typename trainer_type>
void set_c_class1 ( trainer_type& trainer, double C)
{
    pyassert(C > 0, "C must be > 0");
    trainer.set_c_class1(C);
}

template <typename trainer_type>
void set_c_class2 ( trainer_type& trainer, double C)
{
    pyassert(C > 0, "C must be > 0");
    trainer.set_c_class2(C);
}

template <typename trainer_type>
double get_c_class1 ( const trainer_type& trainer) { return trainer.get_c_class1(); }
template <typename trainer_type>
double get_c_class2 ( const trainer_type& trainer) { return trainer.get_c_class2(); }

template <typename trainer_type>
class_<trainer_type> setup_trainer (
    const std::string& name
)
{
    return class_<trainer_type>(name.c_str())
        .def("train", train<trainer_type>)
        .def("set_c", set_c<trainer_type>)
        .def("set_c_class1", set_c_class1<trainer_type>)
        .def("set_c_class2", set_c_class2<trainer_type>)
        .def("get_c_class1", get_c_class1<trainer_type>)
        .def("get_c_class2", get_c_class2<trainer_type>)
        .def("get_epsilon", get_epsilon<trainer_type>)
        .def("set_epsilon", set_epsilon<trainer_type>)
        .def("get_cache_size", get_cache_size<trainer_type>)
        .def("set_cache_size", set_cache_size<trainer_type>);
}

void set_gamma (
    svm_c_trainer<radial_basis_kernel<sample_type> >& trainer,
    double gamma
)
{
    pyassert(gamma > 0, "gamma must be > 0");
    trainer.set_kernel(radial_basis_kernel<sample_type>(gamma));
}

double get_gamma (
    const svm_c_trainer<radial_basis_kernel<sample_type> >& trainer
)
{
    return trainer.get_kernel().gamma;
}

void set_gamma_sparse (
    svm_c_trainer<sparse_radial_basis_kernel<sparse_vect> >& trainer,
    double gamma
)
{
    pyassert(gamma > 0, "gamma must be > 0");
    trainer.set_kernel(sparse_radial_basis_kernel<sparse_vect>(gamma));
}

double get_gamma_sparse (
    const svm_c_trainer<sparse_radial_basis_kernel<sparse_vect> >& trainer
)
{
    return trainer.get_kernel().gamma;
}


// ----------------------------------------------------------------------------------------

void bind_svm_c_trainer()
{
    setup_trainer<svm_c_trainer<radial_basis_kernel<sample_type> > >("svm_c_trainer_radial_basis")
        .def("set_gamma", set_gamma)
        .def("get_gamma", get_gamma);

    setup_trainer<svm_c_trainer<sparse_radial_basis_kernel<sparse_vect> > >("svm_c_trainer_sparse_radial_basis")
        .def("set_gamma", set_gamma_sparse)
        .def("get_gamma", get_gamma_sparse);

    setup_trainer<svm_c_trainer<histogram_intersection_kernel<sample_type> > >("svm_c_trainer_histogram_intersection");

    setup_trainer<svm_c_trainer<sparse_histogram_intersection_kernel<sparse_vect> > >("svm_c_trainer_sparse_histogram_intersection");
}


