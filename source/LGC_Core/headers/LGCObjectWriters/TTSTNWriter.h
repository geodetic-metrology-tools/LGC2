#ifndef SU_TSTN_WRITER
#define SU_TSTN_WRITER

//Copyright 2017 CERN EN/ACE/SU.  All rights reserved.

//SURVEYLIB
#include <TAngle.h>
//LGC
#include <TTSTN.h>
#include <TObservationWriter.h>
#include "LGCAdjustablePoint.h"


/*!
	\ingroup LGCObjectWriters
	\brief Write TSTN  definition and its observations to an LGC output file.
@{*/
class  TTSTNWriter : public TObservationWriter
{
public:
	/// Constructor
	TTSTNWriter(TAStreamFormatter& stream, bool hist);
	///Destructor
	virtual ~TTSTNWriter();


	/*!@name Headers */
	//@{
		/// Writes TSTN header.
		void writeTSTNHeader(shared_ptr<TTSTN> tstn);
		/// Writes V0 (ROM = round of measurement) header.
		void writeV0Header(shared_ptr<TTSTN::TROM> rom);

		/// Write reliability header for ANGL
		void	writeANGLReliabilityHeader();
		/// Write reliability header for ZEND
		void	writeZENDReliabilityHeader();
		/// Write reliability header for DIST
		void	writeDISTReliabilityHeader();
		/// Write reliability header for PLR
		void	writePLRReliabilityHeader();
		/// Write reliability header for DHOR
		void	writeDHORReliabilityHeader();
		/// Write reliability header for ECTH
		void	writeECTHReliabilityHeader();
		/// Write reliability header for ECDIR
		void	writeECDIRReliabilityHeader();

		/// Write header for angle synthesis
		void writeANGLHeaderSynthesis();
		/// Write header for distance and offset synthesis
		void writeDISTHeaderSynthesis();
		/// Write header for plr synthesis
		void writePLRRHeaderynthesis();
	//@}

	/*!@name Results */
	//@{
		///	Writes information about TSTN, its definition, calculated values and each ROM (round of measurements) as well as the observations included.
		void writeTSTNResults(shared_ptr<TTSTN> tstn);
		///	Writes information about TSTN, its definition, calculated values and each ROM (round of measurements) as well as the observations included.
		void writeTSTNResultsSIMU(shared_ptr<TTSTN> tstn);

		/// Write reliability data for ANGL
		void	writeANGLReliabilityData(shared_ptr<TTSTN> tstn, const TLGCStatistic& stat, const std::list<TANGL>& measANGL);
		/// Write reliability data for ZEND
		void	writeZENDReliabilityData(shared_ptr<TTSTN> tstn, const TLGCStatistic& stat, const std::list<TZEND>& measZEND);
		/// Write reliability data for DIST
		void	writeDISTReliabilityData(shared_ptr<TTSTN> tstn, const TLGCStatistic& stat, const std::list<TLINE>& measDIST);
		/// Write reliability data for PLR
		void	writePLRReliabilityData(shared_ptr<TTSTN> tstn, const TLGCStatistic& stat, const std::list<TPLR3D>& measPLR3D);
		/// Write reliability data for DHOR
		void	writeDHORReliabilityData(shared_ptr<TTSTN> tstn, const TLGCStatistic& stat, const std::list<TLINE>& measDHOR);
		/// Write reliability data for ECTH
		void	writeECTHReliabilityData(shared_ptr<TTSTN> tstn, const TLGCStatistic& stat, const std::list<TECTH>& measECTH);
		/// Write reliability data for ECDIR
		void	writeECDIRReliabilityData(shared_ptr<TTSTN> tstn, const TLGCStatistic& stat, const std::list<TECDIR>& measECDIR);


		/// Write ANGL synthesis
        void writeANGLResultsSynthesis(const LGCAdjustablePoint* instrPos, shared_ptr<TTSTN::TROM> rom);
		/// Write ZEND synthesis
        void writeZENDResultsSynthesis(const LGCAdjustablePoint* instrPos, shared_ptr<TTSTN::TROM> rom);
		/// Write DISt and DHOR synthesis
        void writeDISTResultsSynthesis(const LGCAdjustablePoint* instrPos, shared_ptr<TTSTN::TROM> rom, bool isdhor = false);
		/// Write PLR3D synthesis
        void writePLRResultsSynthesis(const LGCAdjustablePoint* instrPos, shared_ptr<TTSTN::TROM> rom);
		/// Write ECTH synthesis
        void writeECTHResultsSynthesis(const LGCAdjustablePoint* instrPos, shared_ptr<TTSTN::TROM> rom);
		/// Write ECDIR synthesis
        void writeECDIRResultsSynthesis(const LGCAdjustablePoint* instrPos, shared_ptr<TTSTN::TROM> rom);
	//@}

	/// Set if ALLFIXED option is used
	void setAllfixed(bool fBool){ isAllfixed = fBool; }

    /// Get the sigma for the distance measurement
    template<class TMEAS>
    static TReal getDistanceSigmaInMM(TMEAS const * const meas){
        // Return value in millimeters [mm]
        return (meas->target.sigmaDist + meas->target.ppmDist * meas->getDistance() / 1000)*M2MM;
    }

    template<class TMEAS>
    static TReal getDistanceSensibility(TMEAS const * const meas, LGCAdjustablePoint const * const instrPos, const TInstrumentData::TPOLAR& instr) {
        TReal dz = meas->targetPos->getEstValue(2) + meas->target.targetHt - instrPos->getEstValue(2) - instr.instrHeight;
        return meas->target.distCorrectionUnknown ?
            10 * dz / (meas->getDistance() + meas->getDistanceResidual() + meas->target.distCorrectionAdjustable->getEstimatedValue()) :
            10 * dz / (meas->getDistance() + meas->getDistanceResidual());
    }

    /// Get the estimated euclidean distance between two points
    static TReal getEstEuclDistance(LGCAdjustablePoint const * const p1, LGCAdjustablePoint const * const p2){
        return sqrt(
            pow2(p1->getEstValue(0) - p2->getEstValue(0)) +
            pow2(p1->getEstValue(1) - p2->getEstValue(1)) +
            pow2(p1->getEstValue(2) - p2->getEstValue(2)));
    }

private:
	/*!@name Headers */
	//@{
		void writeANGLResultsHeader(int nOObs);
		void writeZENDResultsHeader(int nOObs);
		void writeDISTResultsHeader(int nOObs);
		void writePLRResultsHeader(int nOObs);
		void writeDHORResultsHeader(int nOObs);
		void writeECTHResultsHeader(int nOObs);
		void writeECDIRResultsHeader(int nOObs);
	//@}

	/*!@name Results */
	//@{
		void writeANGLResults(const std::list<TANGL>& measANGL, const LGCAdjustablePoint* instrPos, const TAngle& V);
		void writeZENDResults(const std::list<TZEND>& measZEND, const LGCAdjustablePoint* instrPos);
		void writeDISTResults(const std::list<TLINE>& measDIST, const TInstrumentData::TPOLAR& instr, const LGCAdjustablePoint* instrPos);
		void writePLRResults(const std::list<TPLR3D>& measPLR3D, const TInstrumentData::TPOLAR& instr, const LGCAdjustablePoint* instrPos, const TAngle& V, const TAngle& rx, const TAngle& ry);
		void writeDHORResults(const std::list<TLINE>& measDHOR);
		void writeECTHResults(const std::list<TECTH>& measECTH, const LGCAdjustablePoint* instrPos);
		void writeECDIRResults(const std::list<TECDIR>& measECDIR, const LGCAdjustablePoint* instrPos);
	//@}

		bool isAllfixed;
};



#endif //SU_TSTN_WRITER