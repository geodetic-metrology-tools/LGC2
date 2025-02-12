/*
© Copyright CERN 2000-2019. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _TUVD_H_
#define _TUVD_H_

//SURVEYLIB
#include <UEOIndices.h>
//LGC
#include <TAMeas.h>

#if USE_SERIALIZER
#	include <Serializer.hpp>
#endif // USE_SERIALIZER
/*!
	\ingroup Measurements
	\brief Unit vector + distance measurement made by a camera (TCAM).
*/
class TUVD : public TAVectorMeas<TInstrumentData::TCAMD::TTarget> 
{
	public:

		TUVD(const LGCAdjustablePoint& pos, TInstrumentData::TCAMD::TTarget tgt) : 
			TAVectorMeas<TInstrumentData::TCAMD::TTarget>(pos, tgt)
		{
			fMeasIdx.eqDim = 3;
			fMeasIdx.obsDim = 3;
		}

		/// Sets the new distance 
      void setDistance(TLength dist) { sdist = dist; }

      /// Sets the distance residual 
      void setDistanceResidual(TLength sDistRes) { sdistResidual = sDistRes; }

		/// Returns the measured distance 
      TLength getDistance() const { return sdist; }

		/// Returns the distance residual 
      TLength getDistanceResidual() const { return sdistResidual; }
	  virtual void setObsVector(const Eigen::VectorXd &obsVect) override
	  {
		  // internally UVD has 3 residuals: x,y, and distance
		  // zcomp is such that x,y,z is normalized
		  double zObs = sqrt(1 - pow2(obsVect(0)) - pow2(obsVect(1)));
		  TFreeVector direction(obsVect(0), obsVect(1), zObs, TCoordSysFactory::k3DCartesian);
		  setVectorMeasurement(direction);
		  setDistance(TLength(obsVect(2)));
	  }
	  virtual Eigen::VectorXd getObsVector() const override
	  {
		  Eigen::VectorXd result(3);
		  result << getVectorValue().getX().getMetresValue(), getVectorValue().getY().getMetresValue(), getDistance().getMetresValue();
		  return result;
	  }


#if USE_SERIALIZER
	  // Inherited via Serializable
	  virtual void serialize(ObjectSerializer &obj) const override
	  {
		  TAVectorMeas<TInstrumentData::TCAMD::TTarget>::serialize(obj);
		  obj.addProperty("sdist", sdist.getMetresValue());
		  obj.addProperty("sdistResidual", sdistResidual.getMetresValue());
	  }
#endif

	private:
		// distance of the target
      TLength sdist;

		// distance residual of the target
      TLength sdistResidual;
};
#endif