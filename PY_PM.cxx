


#include <iostream>
#include <pybind11/pybind11.h>

// phyton.h was found after using this compile command
// sudo g++ -I/usr/include/python2.7 -lpython2.7 ptest.cxx -o ptest.o
// g++ -I/usr/include/python2.7 -lpython2.7 -O3 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) ptest.cxx -o ptest$(python3-config --extension-suffix)

// g++ -I/usr/include/python2.7 -lpython2.7 -O3 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) ptest.cxx -o ptest.so
// g++ -I/usr/include/python2.7 -lpython2.7 -O3 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) PY_PM.cxx -o PY_PM.so


namespace py = pybind11;

int add( int i, int j ) {
	return i + j;
};

class test {
	public:
	
	int adder(int k, int m);
	test();
};

test::test() {};

int test::adder(int k, int m) {
	
	int retur = k + m;
	return retur;
	
};


PYBIND11_MODULE( PY_PM, m ) {
	//~ m.doc() = "OPC N3 module for RPI using USB ISS";
	//~ m.def("add", &add, "Func");
	
	py::class_<test>(m, "test")
	.def(py::init())
	.def("adder",&test::adder);
	
	
};
