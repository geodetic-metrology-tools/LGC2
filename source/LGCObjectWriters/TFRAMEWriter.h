////////////////////////////////////////////////////////////////////
// TFRAMEWriter
/*!
	Write the FRAME and its definition to an LGC output file, eventually also whole content of the frame (points and observations).
*/
////////////////////////////////////////////////////////////////////////////////////
#ifndef SU_FRAME_WRITER
#define SU_FRAME_WRITER
////////////////////////////////////////////////////////////////
// Forward declarations
#include "TLGCData.h"
#include "TALGCObjectWriter.h"

////////////////////////////////////////////////////////////////

/*!
	\ingroup LGCObjectWriters
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

private:

		/// Writes translation parameter related stuff
		void writeTranslationParameter(const TAdjustableHelmertTransformation& frameDef, int transl);
		/// Writes rotation parameter related stuff
		void writeRotationParameter(const TAdjustableHelmertTransformation& frameDef, int rot);
		///Writes header for point summary
		void writeResultsPtsHeader(const TSpatialStatus::ESpatialStatus status, const int ptNumber, const string &refSys, bool isLocal);

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
		///Transofmation needed if necessary to transform to MLA system
		void transfXYH2XYZ(TPositionVector& pv, const TRefSystemFactory::ERefFrame& rf);

		///Transofmation needed if necessary to transform to MLA system
		void transfXYZ2XYH(TPositionVector& pv, const TRefSystemFactory::ERefFrame& rf);

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