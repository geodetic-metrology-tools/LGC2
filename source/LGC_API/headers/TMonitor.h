#include <iostream>
//STL
#include <random>
#include <TLGCData.h>


using namespace std;
class TMonitor
{
public:
    TMonitor();
    // for FRAS stress test
    void manipulate_ECWS_measurements(TLGCData &data);
    void initialize();
    // void adjust();
	/// Random numbers generator
	std::ranlux48 engine;
    std::shared_ptr<TLGCData> project;
    //TLGCData project;
private:
    // algorithm should be a member that is used by an "adjust method"
    //std::unique_ptr<TVAbstractAlgorithm> algorithm;

};