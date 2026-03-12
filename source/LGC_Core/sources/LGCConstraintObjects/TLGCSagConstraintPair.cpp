#include "TLGCSagConstraintPair.h"

#if USE_SERIALIZER
void TLGCSagConstraintPair::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("refPoint", refPoint);
	obj.addProperty("assocPoint", assocPoint);
	obj.addProperty("sagElementName", fSag.getName());
	obj.addProperty("firstCIndex", firstCIndex);
}
#endif // USE_SERIALIZER
