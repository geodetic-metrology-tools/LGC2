/*
© Copyright CERN 2000-2019. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _TUVEC_H_
#define _TUVEC_H_


//SURVEYLIB
#include <UEOIndices.h>
//LGC
#include <TAMeas.h>

#if USE_SERIALIZER
#	include <Serializer.hpp>
#endif // USE_SERIALIZER
/*!
	\ingroup Measurements
	\brief Unit vector measurement made by a camera (TCAM).
*/
class TUVEC : public TAVectorMeas<TInstrumentData::TCAMD::TTarget> 
{
	public:
		/// Constructor
		TUVEC(const LGCAdjustablePoint &pos, TInstrumentData::TCAMD::TTarget tgt) : TAVectorMeas<TInstrumentData::TCAMD::TTarget>(pos, tgt)
		{
			fMeasIdx.eqDim = 2;
			fMeasIdx.obsDim = 2;
		}
				
		virtual void setObsVector(const Eigen::VectorXd &obsVect) override
		{
			// internally UVEC has 2 residuals: x,y
			// zcomp is such that x,y,z is normalized
			double zObs = sqrt(1 - pow2(obsVect(0)) - pow2(obsVect(1)));
			TFreeVector direction(obsVect(0), obsVect(1), zObs, TCoordSysFactory::k3DCartesian);
			setVectorMeasurement(direction);
		}

		virtual Eigen::VectorXd getObsVector() const override
		{
			Eigen::VectorXd result(2);
			result << getVectorValue().getX().getMetresValue(), getVectorValue().getY().getMetresValue();
			return result;
		}

#if USE_SERIALIZER
		// Inherited via Serializable
		virtual void serialize(ObjectSerializer &obj) const override
		{ 
			TAVectorMeas<TInstrumentData::TCAMD::TTarget>::serialize(obj);
		}
#endif
};
#endif