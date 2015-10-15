#include "TDist.h"

/////////////////////////////////
// planimetric distance in meters
/////////////////////////////////
TReal	dist(TLength x1, TLength y1, TLength x2, TLength y2)
{
	
	return sqrtq(powq((x1.getMetresValue()-x2.getMetresValue()),2)
		+powq((y1.getMetresValue()-y2.getMetresValue()),2));
}


TReal	dist(TReal x1, TReal y1, TReal x2, TReal y2)
{
	
	return sqrtq(powq((x1-x2),2)+powq((y1-y2),2));
}


TReal	dist3D(TLength x1, TLength y1, TLength z1, TLength x2, TLength y2, TLength z2)
{
	
	return sqrtq(powq((x1.getMetresValue()-x2.getMetresValue()),2)
		+powq((y1.getMetresValue()-y2.getMetresValue()),2) +powq((z1.getMetresValue()-z2.getMetresValue()),2));


}


TReal	dist3D(TReal x1, TReal y1, TReal z1, TReal x2, TReal y2, TReal z2)
{
	return sqrtq(powq((x1-x2),2)
		+powq((y1-y2),2) +powq((z1-z2),2));
}
