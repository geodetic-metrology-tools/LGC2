#ifndef TPOINTTRANSFORMER_H
#define TPOINTTRANSFORMER_H

#include "TCCS2CGRFTransformation.h"
#include "TCGRF2LGTransformation.h"
#include "TILG2ILATransformation.h"
#include "TLA2MLATransformation.h"

#include "TRefSystemFactory.h"
#include "TLOR2LOR.h"



/*!
\ingroup PointTransformationFunctions

\brief Functions to manage point transformations
*/
class TPointTransformer{

public:

	/*!@name Constructors*/
	//@{
	/*!
	\param[in] tree A tree of local object reference frames.
	\param[in] refFrame A reference frame used in the project.
	*/
	TPointTransformer(const TDataTree* tree, const TRefSystemFactory::ERefFrame& refFrame);
	//@}

	/*!
	\brief Updates the transformations between nodes, stored in the 'fLORTrafo' vector.

	The transformation parametres stored in the tree can change during the adjustment, therefore update of the transformations is REQUIRED.
	This method therefore HAVS TO BE CALLED whenever the TREE of local frames CHANGED, typically in the beginning of Filling Input Matrices procedure.
	*/
	void updateTransformations();

	/*!@name LOR Transformations stuff */
	//@{
	/// Vector containing all transformations between LORs of the tree, which were used
	std::list<TLOR2LOR> fLORTrafo;

	///  Returns the the index of a transformation named 'transfName' in the 'fLORTrafo' vector.  Returns the -1 if transformation was not set up.
	int getTransformationIndex(const std::string& transfName) const;

	/// Returns the an identity transformation
	TLOR2LOR getIdentityTransformation();

	// Returns the a transformation from 'from' node into 'to' node
	// Be carefoul, in a case it is called for second time in a function and the transformation is not yet in the vector, it is reallocated and the reference to the first transformation is lost.
	const TLOR2LOR& getLORTransformation(TDataTreeIterator from, TDataTreeIterator to);

	/// Transform points into an MLA system, where the first point is the origin of the system.
	void transformPointsToMLASystem(std::string originName, TPositionVector& originOfMLAPos, TPositionVector& additPointPos);
	//@}
	
	/*!@name CCS2MLA related stuff */
	//@{
	///Sets new origin of the transformation to MLA system
	void set2MLATransformation(TPositionVector originInCCS);

	///Transform a free vector fromo a MLA system to CGRF - used only for the dver measurements
	void transformMLA2CGRF(TFreeVector& fv);

	///Transform a position vector into a MLA system
	void transform2MLA(TPositionVector& pv);

	///Transform a free vector into a MLA system
	void transform2MLA(TFreeVector& pv);

	void setMLA(bool fBool){ fMLAused = fBool; }
	void setCGRF(bool fBool){ fCGRFused = fBool; }

	bool getMLAused(){ return fMLAused; }
	bool getCGRFused(){ return fCGRFused; }
	bool getIsSphere(){ return fIsSphere; }

	TCCS2CGRFTransformation& getCCS2CGRF(){ return fccs2cgrf; }
	TLA2MLATransformation& getLA2MLA(){ return fla2mla; }
	//@}

	//Pointer to a tree of LORs
	const TDataTree* getTree() {
		return fTree;
	}

	const TRefSystemFactory::ERefFrame&  getRefFrame() { return fRefFrame; }

	const std::string&  getLastStnPtName(){ return   fLastStationPtName; }

	void  setLastStnPtName(std::string fName){ fLastStationPtName = fName; }

private:
	//Pointer to a tree of LORs
	const TDataTree* fTree;

	//Reference to the reference frame used (Stored in the TLGCData configuration)
	const TRefSystemFactory::ERefFrame fRefFrame;

	//Geoid model used, determined by the fRefFrame
	TRefSystemFactory::EGeoid fGeoidModel;


	/*!@name CCS2MLA related stuff */
	//@{
	//Transformation between reference frames
	TCCS2CGRFTransformation fccs2cgrf;
	TCGRF2LGTransformation fcgrf2ilg;
	TILG2ILATransformation  filg2ila;
	TLA2MLATransformation fla2mla;

	

	/// Name of the last processed point which was used as the origin of the MLA.
	// If a transformation to MLA is needed, we firstly compare the name of the new origin point with this name.
	// If they are the same it means that the MLA transformation which is currently initialized does not need to be re-initialized.
	std::string     fLastStationPtName;

	///Tells whether last processed contribution was  made in the Modified Local Astronomical (MLA) system 
	bool			fMLAused;
	///Tells whether last processed contribution was  made in the CGRF system 
	bool			fCGRFused; /*for DVER*/

	bool fIsSphere;
	//@}
};
#endif 
