/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _TLGCREFRAME_H_
#define _TLGCREFRAME_H_

#include "Global.h"

/*! 
	Thin wrapper around Surveylib to ease a clean integration

	\note Should be replaced by a respcetive SurveyLib class.
*/
class TLGCRefFrame {
	public:
		enum ERefs {
			kOLOC,
			kRS2K,
			kLEP,
			kSPHE,
			kNONE
		};

		TLGCRefFrame(ERefs rf = kNONE) : frf(rf) {}

		inline const ERefs& typeRef() const {
			return frf;
		}


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
				default:
					return "NONE";
			}
		}

	private:
		ERefs frf;
};

#endif
