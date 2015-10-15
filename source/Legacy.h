#ifndef _LGCLEGACY_H_
#define _LGCLEGACY_H_

#include "Global.h"

/// Thin wrapper around Surveylib to ease a clean integration
class TLGCRefFrame {
	public:
		enum ERefs {
			kOLOC,
			kRS2K,
			kLEP,
			kSPHE,
			//kCCS,
			//kLA,
			//kLG,
			kNONE
		};

		TLGCRefFrame(ERefs rf = kNONE) : frf(rf) {}

		inline bool isCartesian() const {
			return (frf == kOLOC);
		}
		
		inline ERefs type() const {
			return frf;
		}

		inline std::string typeName() const {
			switch(frf){
				case kOLOC:
					return "LOCAL";
				case kRS2K:
					return "RS2K";
				case kLEP:
					return "LEP";
				case kSPHE:
					return "SPHE";
				case kNONE:
					return "NONE";
			}
		}

	//	void transform(TReal point[3], const TLGCRefFrame& to);

	private:
		ERefs frf;

	//	struct D;
		//std::unique_ptr<D> d;
};

#endif
