// TLSCalcWorkingPosVec.h

#ifndef SU_LS_WORK_POSVEC
#define SU_LS_WORK_POSVEC

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "TLSCalcPosVectorParam.h"
#include "lsalgo/TLSCalcWorkingUnknown.h"

class TLSCalcWorkingPosVec : public TLSCalcWorkingUnknown<TLSCalcPosVectorParam> {

public:

	LSPosVecIter		insert(const LSPosVecIter, const TLSCalcPosVectorParam& posVec);

	/*!\return an iterator pointing to a point specified through its name*/
	LSPosVecIter		getPoint(const string);

	/*!\return a cons iterator pointing to a point specified through its name*/
	LSPosVecConstIter	getPoint(const string) const;

	/*!function used for order posVecParam like points in thr input file
	Move Pos2 to pos1*/
	void				splice(const LSPosVecIter pos1, const LSPosVecIter pos2) { paramsList.splice(pos1, paramsList, pos2); }

	/*!reverse the list*/
	void				reverse() { paramsList.reverse(); }

	//@}

};

#endif //SU_LS_WORK_POSVEC
