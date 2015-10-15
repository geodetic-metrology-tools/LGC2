//  TXYHs2CCS.h
#ifndef SU_XYHs_2_CCS
#define SU_XYHs_2_CCS

#include "TRefSystemFactory.h"

// TRefSystemFactory is used for transformation between XYHg and XYHe systems, using Geoid model defined in it. 

/*!
	\brief This class provide static methods for transformations between XYH systems and the CCS.
*/
class  TXYH2CCS
{
public:
		static bool						XYHs2CCS( TPositionVector& pv );

		static bool						CCS2XYHs(TPositionVector& pv);

		static bool						XYHg2000Machine2CCS( TPositionVector& pv );

		static bool						CCS2XYHg2000Machine( TPositionVector& pv );

		static bool						XYHg1985Machine2CCS(TPositionVector& pv);

		static bool						CCS2XYHg1985Machine(TPositionVector& pv);



private:
		static bool						XYHe2CCS( TPositionVector& pv );

		static bool						CCS2XYHe( TPositionVector& pv );

		static bool						XYHg2XYHe( TPositionVector& pv, TRefSystemFactory::EGeoid geoid);

		static bool						XYHe2XYHg( TPositionVector& pv , TRefSystemFactory::EGeoid geoid);

};

#endif // TXYHs2CCS.h
