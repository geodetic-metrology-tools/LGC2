/*
© Copyright CERN 2000-2017. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef TPOINTTRANSFORMER_H
#define TPOINTTRANSFORMER_H


//SURVEYLIB
#include "TRefSystemFactory.h"
//LGC
#include "TCCS2CGRFTransformation.h"
#include "TCGRF2LGTransformation.h"
#include "TILG2ILATransformation.h"
#include "TLA2MLATransformation.h"
#include "TLOR2LOR.h"


/*!
\ingroup ContributionsGenerators

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
	This method therefore HAVE TO BE CALLED whenever the TREE of local frames CHANGED, typically in the beginning of Filling Input Matrices procedure.
	*/
	void updateTransformations();

	/*!@name LOR Transformations stuff */
	//@{
	
	/*!
	\brief Returns the a transformation from 'from' node into 'to' node
	 Be carefoul, in a case it is called for second time in a function and the transformation is not yet in the vector, it is reallocated and the reference to the first transformation is lost.
	*/
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

	/// Sets the boolean value to tell is MLA is used
	void setMLA(bool fBool){ fMLAused = fBool; }

	/// Sets the boolean value to tell is CGRF is used
	void setCGRF(bool fBool){ fCGRFused = fBool; }

	/// Gets the boolean value to tell is MLA is used
	bool getMLAused(){ return fMLAused; }

	/// Gets the boolean value to tell is CGRF is used
	bool getCGRFused(){ return fCGRFused; }

	/// Returns the transformation from CCS to CGRF
	TCCS2CGRFTransformation& getCCS2CGRF(){ return fccs2cgrf; }

	/// Returns the transformation from LA to MLA
	TLA2MLATransformation& getLA2MLA(){ return fla2mla; }
	//@}

	/// Pointer to a tree of LORs
	const TDataTree* getTree() { return fTree;}

	/// Returns a reference on the reference frame
	const TRefSystemFactory::ERefFrame&  getRefFrame() { return fRefFrame; }

	/// Returns a reference on the name of the last station used
	const std::string&  getLastStnPtName(){ return   fLastStationPtName; }

	/// Sets the name of the last station used
	void  setLastStnPtName(std::string fName){ fLastStationPtName = fName; }

    TPointTransformer& operator=(const TPointTransformer &other) = delete;

private:
	///  Returns the the index of a transformation named 'transfName' in the 'fLORTrafo' vector.  Returns the -1 if transformation was not set up.
	int getTransformationIndex(const std::string& transfName) const;

	/// Returns an identity transformation
	TLOR2LOR getIdentityTransformation();

	/// Pointer to a tree of LORs
	const TDataTree* fTree;

	/// The reference frame used (Stored in the TLGCData configuration)
	const TRefSystemFactory::ERefFrame fRefFrame;

	/// Geoid model used, determined by the fRefFrame
	TRefSystemFactory::EGeoid fGeoidModel;

	/// Vector containing all transformations between LORs of the tree, which were used
	std::list<TLOR2LOR> fLORTrafo;

	/// Transformation between reference frames
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

	///Tells if SPHE reference frame is used
	bool fIsSphere;
};
#endif 
