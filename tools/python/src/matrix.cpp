
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>
#include <dlib/matrix.h>
#include "serialize_pickle.h"


using namespace dlib;
using namespace std;
using namespace boost::python;


void matrix_set_size(matrix<double>& m, long nr, long nc)
{
    m.set_size(nr,nc);
    m = 0;
}

string matrix_double__str__(matrix<double>& c)
{
    ostringstream sout;
    sout << c;
    return sout.str();
}

boost::shared_ptr<matrix<double> > make_matrix_from_size(long nr, long nc)
{
    boost::shared_ptr<matrix<double> > temp(new matrix<double>(nr,nc));
    *temp = 0;
    return temp;
}


boost::shared_ptr<matrix<double> > from_object(object obj)
{
    tuple s = extract<tuple>(obj.attr("shape"));
    if (len(s) != 2)
    {
        PyErr_SetString( PyExc_IndexError, "Input must be a matrix or some kind of 2D array." 
        );                                            
        boost::python::throw_error_already_set();   
    }

    const long nr = extract<long>(s[0]);
    const long nc = extract<long>(s[1]);
    boost::shared_ptr<matrix<double> > temp(new matrix<double>(nr,nc));
    for ( long r = 0; r < nr; ++r)
    {
        for (long c = 0; c < nc; ++c)
        {
            (*temp)(r,c) = extract<double>(obj[make_tuple(r,c)]);
        }
    }
    return temp;
}

long matrix_double__len__(matrix<double>& c)
{
    return c.nr();
}


struct mat_row
{
    mat_row() : data(0),size(0) {}
    mat_row(double* data_, long size_) : data(data_),size(size_) {}
    double* data;
    long size;
};

void mat_row__setitem__(mat_row& c, long p, double val)
{
    if (p < 0) {
        p = c.size + p; // negative index
    }
    if (p > c.size-1) {
        PyErr_SetString( PyExc_IndexError, "3 index out of range" 
        );                                            
        boost::python::throw_error_already_set();   
    }
    c.data[p] = val;
}


string mat_row__str__(mat_row& c)
{
    ostringstream sout;
    sout << mat(c.data,1, c.size);
    return sout.str();
}

long mat_row__len__(mat_row& m)
{
    return m.size;
}

double mat_row__getitem__(mat_row& m, long r)
{
    if (r < 0) {
        r = m.size + r; // negative index
    }
    if (r > m.size-1 || r < 0) {
        PyErr_SetString( PyExc_IndexError, "1 index out of range" 
        );                                            
        boost::python::throw_error_already_set();   
    }
    return m.data[r];
}

mat_row matrix_double__getitem__(matrix<double>& m, long r)
{
    if (r < 0) {
        r = m.nr() + r; // negative index
    }
    if (r > m.nr()-1 || r < 0) {
        PyErr_SetString( PyExc_IndexError, (string("2 index out of range, got ") + cast_to_string(r)).c_str()
        );                                            
        boost::python::throw_error_already_set();   
    }
    return mat_row(&m(r,0),m.nc());
}


tuple get_matrix_size(matrix<double>& m)
{
    return make_tuple(m.nr(), m.nc());
}

void bind_matrix()
{
    class_<mat_row>("_row")
        .def("__len__", &mat_row__len__)
        .def("__repr__", &mat_row__str__)
        .def("__str__", &mat_row__str__)
        .def("__setitem__", &mat_row__setitem__)
        .def("__getitem__", &mat_row__getitem__);

    class_<matrix<double> >("matrix", init<>())
        .def("__init__", make_constructor(&make_matrix_from_size))
        .def("set_size", &matrix_set_size)
        .def("__init__", make_constructor(&from_object))
        .def("__repr__", &matrix_double__str__)
        .def("__str__", &matrix_double__str__)
        .def("__len__", &matrix_double__len__)
        .def("__getitem__", &matrix_double__getitem__, with_custodian_and_ward_postcall<0,1>())
        .add_property("shape", &get_matrix_size)
        .def_pickle(serialize_pickle<matrix<double> >());
}

