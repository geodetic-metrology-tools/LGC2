#include "dist3D.h"

/////////////////////////////////
// planimetric distance in meters
/////////////////////////////////
double	dist3D(TLength x1, TLength y1, TLength z1, TLength x2, TLength y2, TLength z2){
	
	return sqrt(pow((x1.getMetresValue()-x2.getMetresValue()),2)
		+pow((y1.getMetresValue()-y2.getMetresValue()),2) +pow((z1.getMetresValue()-z2.getMetresValue()),2));
}