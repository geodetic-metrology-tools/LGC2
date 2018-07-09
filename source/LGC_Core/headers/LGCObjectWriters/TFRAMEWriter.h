
/*
© Copyright CERN 2000-2017. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef SU_FRAME_WRITER
#define SU_FRAME_WRITER

//LGC
#include <TLGCData.h>
#include <TALGCObjectWriter.h>

/*!
	\ingroup LGCObjectWriters
	\brief Write the FRAME and its definition to an LGC output file, eventually also whole content of the frame (points and observations).
	
@{*/
class  TFRAMEWriter : public TALGCObjectWriter 
{
public:
	/// Constructor
	TFRAMEWriter(TAStreamFormatter& stream, const TLGCData* data);

	///Destructor
	virtual ~TFRAMEWriter();


	///	Writes all information about frame, its definition, calculated parameters, points defined in this frame and results of all observation belonging to this frame.
	void writeFRAMEAll(TDataTreeIterator frameIt);

	/// Write frame for the simulations
	void writeFRAMESimu(TDataTreeIterator frameIt);

	///	Writes all reliability parameters of all observation belonging to this frame.
	void writeFRAMEAllReliability(TDataTreeIterator frameIt);

	/// Writes the frame header.
	void writeFRAMEHeader(const std::string& name, const std::vector<int>& ID);

	/// Writes the frame definition.
	void writeFRAMEDefinition(const TTreeEntry& frame);

	/*! 
		\brief Writes all points defined in this frame. 
		If it is a ROOT frame, write all points defined within whole project transformed in the ROOT.
	*/
	void writePoints(TDataTreeIterator frameIt);

    ///Transformation needed if necessary to transform to MLA system
    static void transfXYH2XYZ(TPositionVector& pv, const TRefSystemFactory::ERefFrame& rf);
    ///Transformation needed if necessary to transform to MLA system
    static void transfXYZ2XYH(TPositionVector& pv, const TRefSystemFactory::ERefFrame& rf);

private:

		/// Writes translation parameter related stuff
		void writeTranslationParameter(const TAdjustableHelmertTransformation& frameDef, int transl);
		/// Writes rotation parameter related stuff
		void writeRotationParameter(const TAdjustableHelmertTransformation& frameDef, int rot);
		///Writes header for point summary
		void writeResultsPtsHeader(const TSpatialStatus::ESpatialStatus status, const int ptNumber, const std::string &refSys, bool isLocal);

		/// Write TSTSN reliability
		void writeTSTNReliability(TDataTreeIterator frameIt);
		/// Write CAM reliability
		void writeCAMReliability(TDataTreeIterator frameIt);
		/// Write EDM reliability
		void writeEDMReliability(TDataTreeIterator frameIt);
		/// Write LEVEL reliability
		void writeLEVELReliability(TDataTreeIterator frameIt);
		/// Write SCALE reliability
		void writeSCALEReliability(TDataTreeIterator frameIt);

		///Writes specific point
		void writeResultsPtsData(AdjPointIter pt, bool isLocal);
		///Writes points of the same type
		void writePointType(const std::list<AdjPointIter>& lop, TDataTreeIterator frameIt, TSpatialStatus::ESpatialStatus type, bool localNode);
		
		///write measurements summary
		void writeMeasurementsSummary(TDataTreeIterator frameIt);
		///write measurements summary
		void writeHistogramme(TDataTreeIterator frameIt);
		
		///WriteEllipsHeader
		void writeEllipsHeader();
		///WriteEllipsoidHeader
		void writeEllipsoidHeader();
		///WriteEllipsData
		void writeEllipsData(AdjPointIter& pt);
		///WriteEllipsoidData
		void writeEllipsoidData(AdjPointIter& pt);

		/// Project data
		const TLGCData* fProjectData; 

		//Lists of iterators to the point collection, separated into lists according to they type. Need to be written out to output according to the type.
		std::list<AdjPointIter> pointCALA;
		std::list<AdjPointIter> pointVXYZ;
		std::list<AdjPointIter> pointVXY;
		std::list<AdjPointIter> pointVXZ;
		std::list<AdjPointIter> pointVYZ;
		std::list<AdjPointIter> pointVZ;
};

#endif //SU_FRAME_WRITER