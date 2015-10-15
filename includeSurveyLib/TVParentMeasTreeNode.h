
#ifndef SU_TVPARENTMEASTREENODE
#define SU_TVPARENTMEASTREENODE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


//*********
// Forward declarations
#include "TVMeasurementTreeNode.h"
//
// typedefs
//
//*********


//! Pure Abstract Base Class defining an interface for the parent measurement tree nodes.
/*!Implemented by every station and round of measurements. */
class	TVParentMeasTreeNode : public TVMeasurementTreeNode{

public:	

	//! Responds to the disactivation of a child measurement tree node
	virtual void		childDisactivated(int measCount, int eqCount, int unkCount) = 0;
	//! Responds to the activation of a child measurement tree node
	virtual void		childActivated(int measCount, int eqCount, int unkCount) = 0;
	
	virtual void	sendEnoughTotalEqns() = 0;
	virtual void	sendDisactivatedToChildren() = 0;
	virtual void	sendActivatedToChildren() = 0;

	virtual void	childReadyToActivate(int measCount, int eqCount, int unkCount) = 0;
	virtual bool	enoughTotalEqns() const = 0;
	virtual void	activateReadyChildren() = 0;
};
#endif
