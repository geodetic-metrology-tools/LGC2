#ifndef SU_GRID_GEOID_MODELS
#define SU_GRID_GEOID_MODELS

#include <TCernGridGeoid.h>
#include <TReferenceEllipsoid.h>

class TGridGeoidModels{
	public:
		enum EGridType {pCG2000Machine, kCGSphere, kCG1985, kCG1985Machine, kNoGeoid};
		TGridGeoidModels();
		/// Destructor
		~TGridGeoidModels();

	
		vector<TAGeoidModel*> fGeoidList;	/*!< list of pointers to the to-be-defined geoids */
		vector<TReferenceEllipsoid*> fRefEllList; /*!< list of pointers to the to-be-defined ellipsoids */
		vector<TAReferenceFrame*> fRefFrameList; /*!< list of pointers to the to-be-defined Ref.Frames */
};
#endif