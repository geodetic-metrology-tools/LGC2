////////////////////////////////////////////////////////////////////
// TFRAMEConverter
/*!
	Write the FRAME and its definition to an LGC output file, eventually also whole content of the frame (points and observations).
*/
////////////////////////////////////////////////////////////////////////////////////
#ifndef SU_FRAME_CONVERTER
#define SU_FRAME_CONVERTER
////////////////////////////////////////////////////////////////
// Forward declarations
#include "TLGCData.h"
#include "TALGCConverter.h"

////////////////////////////////////////////////////////////////
typedef std::list<TAdjustablePoint>::const_iterator AdjPointIter;


//Class definition
class  TFRAMEConverter : public TALGCConverter 
{
public:
	//! Constructor
	TFRAMEConverter(TAStreamFormatter& stream, const TLGCData* data);

	//!Destructor
	virtual ~TFRAMEConverter();

	/*!
		Writes all information about frame, its definition, calculated parameters, points defined in this frame and results of all observation belonging to this frame.
	*/
	void writeFRAMEAll(TDataTreeIterator frameIt);

	//! Writes the frame header.
	void writeFRAMEHeader(const std::string& name, const std::vector<int>& ID);

	//! Writes the frame definition.
	void writeFRAMEDefinition(const TTreeEntry& frame);

	/*! 
		Writes all points defined in this frame. 
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

		///Writes specific point
		void writeResultsPtsData(AdjPointIter pt, bool isLocal);
		///Writes points of the same type
		void writePointType(const std::list<AdjPointIter>& lop, TDataTreeIterator frameIt, TSpatialStatus::ESpatialStatus type, bool localNode);
		///Transofmation needed if necessary to transform to MLA system
		void transfUsingGeoid(TPositionVector& pv, const TLGCRefFrame::ERefs& rf);
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

#endif //SU_FRAME_CONVERTER