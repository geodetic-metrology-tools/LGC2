#include <iostream>
using namespace std;

#include "TLGCApplication.h"

int main( int argc, char** argv){
	
	// initialize openmp
	omp_set_num_threads(omp_get_num_procs());

	TLGCApplication lgc(argc, argv);
	
	return lgc.exec();
}
