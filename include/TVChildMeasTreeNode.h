
#ifndef SU_TVCHILDMEASTREENODE
#define SU_TVCHILDMEASTREENODE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


//////////////////////////////////
// Forward declarations
#include <TVMeasurementTreeNode.h>
//
// typedefs
//
//////////////////////////////////


//! Pure Abstract Base Class defining an interface for the children measurement tree nodes.
/*!Implemented by every round of measurements and every measurement. */
class	TVChildMeasTreeNode : public TVMeasurementTreeNode{

public:	
	
	//! Responds to the disactivation of the parent node
	virtual void		parentDisactivated()  = 0;
	//! Responds to the activation of the parent node
	virtual void		parentActivated() = 0;
	virtual void		parentEnoughTotalEqns() = 0;

	virtual void		sendDisactivatedToParent() = 0;
	virtual void		sendActivatedToParent() = 0;
};
#endif
