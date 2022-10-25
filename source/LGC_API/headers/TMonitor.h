#include <iostream>
//STL
#include <random>
#include <TLGCData.h>


using namespace std;
class TMonitor
{
public:
    void BasicObject();
    // for FRAS stress test
    void manipulate_ECWS_measurements(TLGCData *data);
	/// Random numbers generator
	std::ranlux48 engine;
};