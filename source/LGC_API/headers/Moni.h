
#ifndef MONI
#define MONI


#include <Eigen/Dense>
#include <string>

class Moni
{
public:
	Moni(std::string inputFilePath);
	~Moni();
	//void updateMeas(std::string id, Eigen::VectorXd measurementVector) { pimpl_->updateMeas(id, measurementVector); }
	
private:
	class MoniImpl;
	std::unique_ptr<MoniImpl> pimpl_;
};



#endif
