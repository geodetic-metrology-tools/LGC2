#include <TRefFrameInfo.h>

#include <memory> // for std::auto_ptr
#include <stdexcept>
#include <utility> // for std::make_pair

TRefFrameInfo::TDetails::TDetails(
								  const std::string & refFrameName,
                                  const std::string & name, 
                                  TCoordSysFactory::ECoordSys defaultCoordSys,
                                  int allowedCoordsys,
                                  bool local)
: fRefFrameName(refFrameName)
, fName(name)
, fDefaultCoordSys(defaultCoordSys)
, fAllowedCoordSys(allowedCoordsys|defaultCoordSys)
, fLocal(local)
{
}

const TRefFrameInfo::MappingType & TRefFrameInfo::getMapping()
{
	typedef std::auto_ptr<MappingType> MapPtr;
	static MapPtr mapping;
	if(mapping.get()==NULL)
	{
		MapPtr tmp = MapPtr(new MappingType);
		tmp->insert(std::make_pair(TRefSystemFactory::kCCS, 
            TDetails("kCCS","CCS - CERN XYZ", TCoordSysFactory::k3DCartesian)));
        tmp->insert(std::make_pair(TRefSystemFactory::kLAp0, 
            TDetails("kLAp0","LAp0", TCoordSysFactory::k3DCartesian)));
        tmp->insert(std::make_pair(TRefSystemFactory::kLGp0, 
            TDetails("kLGp0","LGp0", TCoordSysFactory::k3DCartesian)));
        tmp->insert(std::make_pair(TRefSystemFactory::kCGRF, 
            TDetails("kCGRF","CERN GRF", TCoordSysFactory::k3DCartesian, TCoordSysFactory::kGeodetic)));
        tmp->insert(std::make_pair(TRefSystemFactory::kITRF97, 
            TDetails("kITRF97","ITRF97 ep98.5", TCoordSysFactory::k3DCartesian, TCoordSysFactory::kGeodetic)));
        tmp->insert(std::make_pair(TRefSystemFactory::kWGS84, 
            TDetails("kWGS84","WGS84", TCoordSysFactory::k3DCartesian, TCoordSysFactory::kGeodetic)));
        tmp->insert(std::make_pair(TRefSystemFactory::kROMA40, 
            TDetails("kROMA40","ROMA40", TCoordSysFactory::k3DCartesian, TCoordSysFactory::kGeodetic)));
		tmp->insert(std::make_pair(TRefSystemFactory::kETRF93,
            TDetails("kETRF93","ETRF93", TCoordSysFactory::k3DCartesian, TCoordSysFactory::kGeodetic)));
        tmp->insert(std::make_pair(TRefSystemFactory::kCH1903plus,
            TDetails("kCH1903plus","CH1903+", TCoordSysFactory::k3DCartesian, TCoordSysFactory::kGeodetic)));
		 
		 
        tmp->insert(std::make_pair(TRefSystemFactory::kCernXYHe, 
            TDetails("kCernXYHe","CERN XYHe", TCoordSysFactory::k2DPlusH)));
        tmp->insert(std::make_pair(TRefSystemFactory::kCernX0Y0He, 
            TDetails("kCernX0Y0He","CERN XoYoHe (Map Transfer)", TCoordSysFactory::k2DPlusH)));
        tmp->insert(std::make_pair(TRefSystemFactory::kCernXYHg00, 
            TDetails("kCernXYHg00","CERN XYHg00", TCoordSysFactory::k2DPlusH)));
        tmp->insert(std::make_pair(TRefSystemFactory::kCernXYHg00Topo, 
            TDetails("kCernXYHg00Topo","CERN XYHg00Topo", TCoordSysFactory::k2DPlusH)));
        tmp->insert(std::make_pair(TRefSystemFactory::kCernXYHg00Machine, 
            TDetails("kCernXYHg00Machine","CERN XYHg (RS2K)", TCoordSysFactory::k2DPlusH)));
        tmp->insert(std::make_pair(TRefSystemFactory::kCernXYHg85, 
            TDetails("kCernXYHg85","CERN XYHg1985 (Surface Topo)", TCoordSysFactory::k2DPlusH)));
        tmp->insert(std::make_pair(TRefSystemFactory::kCernXYHg85Machine, 
            TDetails("kCernXYHg85Machine","CERN XYHg1985 (LHC)", TCoordSysFactory::k2DPlusH)));
        tmp->insert(std::make_pair(TRefSystemFactory::kCERNXYHsSphereSPS, 
            TDetails("kCERNXYHsSphereSPS","CERN XYHs (SPS)", TCoordSysFactory::k2DPlusH)));
        tmp->insert(std::make_pair(TRefSystemFactory::kCGRFSphere,
            TDetails("kCGRFSphere","CERN GRF sphere", TCoordSysFactory::k3DCartesian, TCoordSysFactory::kGeodetic)));
#ifdef USE_SWISSTOPO
        tmp->insert(std::make_pair(TRefSystemFactory::kSwissLV95,
            TDetails("kSwissLV95","Swiss LV95 (ellipsoidal height)", TCoordSysFactory::k2DPlusH)));
        tmp->insert(std::make_pair(TRefSystemFactory::kSwissLV03,
            TDetails("kSwissLV03","Swiss LV03 (ellipsoidal height)", TCoordSysFactory::k2DPlusH)));
#endif
        tmp->insert(std::make_pair(TRefSystemFactory::kFrenchRGF93Zone5,
            TDetails("kFrenchRGF93Zone5","French RGF93zone5 (ellipsoidal height)", TCoordSysFactory::k2DPlusH)));
		tmp->insert(std::make_pair(TRefSystemFactory::kLambert93,
			TDetails("kLambert93","French Lambert93", TCoordSysFactory::k2DPlusH)));
		//RGF93 CHTRF95
		tmp->insert(std::make_pair(TRefSystemFactory::kRGF93,
            TDetails("kRGF93","RGF93", TCoordSysFactory::k3DCartesian, TCoordSysFactory::kGeodetic)));
		tmp->insert(std::make_pair(TRefSystemFactory::kCHTRF95,
            TDetails("kCHTRF95","CHTRF95", TCoordSysFactory::k3DCartesian, TCoordSysFactory::kGeodetic)));
		//


        tmp->insert(std::make_pair(TRefSystemFactory::kMLA1985Machine, 
            TDetails("kMLA1985Machine","MLA (1985)", TCoordSysFactory::k3DCartesian, 0, true)));
        tmp->insert(std::make_pair(TRefSystemFactory::kMLA2000Machine, 
            TDetails("kMLA2000Machine","MLA (2000)", TCoordSysFactory::k3DCartesian, 0, true)));
        tmp->insert(std::make_pair(TRefSystemFactory::kMLASphere, 
            TDetails("kMLASphere","MLA (Sphere)", TCoordSysFactory::k3DCartesian, 0, true)));
        tmp->insert(std::make_pair(TRefSystemFactory::kLA1985Machine, 
            TDetails("kLA1985Machine","LA (1985)", TCoordSysFactory::k3DCartesian, 0, true)));
        tmp->insert(std::make_pair(TRefSystemFactory::kLA2000Machine, 
            TDetails("kLA2000Machine","LA (2000)", TCoordSysFactory::k3DCartesian, 0, true)));
        tmp->insert(std::make_pair(TRefSystemFactory::kLASphere, 
            TDetails("kLASphere","LA (Sphere)", TCoordSysFactory::k3DCartesian, 0, true)));

		
		tmp->insert(std::make_pair(TRefSystemFactory::kMLGGRS80, 
			TDetails("kMLGGRS80","MLG", TCoordSysFactory::k3DCartesian, 0, true)));
		tmp->insert(std::make_pair(TRefSystemFactory::kMLGSphere, 
			TDetails("kMLGSphere","MLG (Sphere)", TCoordSysFactory::k3DCartesian, 0, true)));
		tmp->insert(std::make_pair(TRefSystemFactory::kLGGRS80, 
			TDetails("kLGGRS80","LG ", TCoordSysFactory::k3DCartesian, 0, true)));
		tmp->insert(std::make_pair(TRefSystemFactory::kLGSphere, 
			TDetails("kLGSphere","LG (Sphere)", TCoordSysFactory::k3DCartesian, 0, true)));

        tmp->insert(std::make_pair(TRefSystemFactory::kLocalRefFrame, 
            TDetails("kLocalRefFrame","LocalRefFrame (RESERVED)", TCoordSysFactory::k3DCartesian, 0, true)));
		// ...
		mapping = tmp;
	}
	return *mapping;
}

const TRefFrameInfo::RefFrameSet & TRefFrameInfo::getRefFrameSet()
{
	typedef std::auto_ptr<RefFrameSet> SetPtr;
	static SetPtr refFrameSet;
	if(refFrameSet.get()==NULL)
	{
		SetPtr tmp = SetPtr(new RefFrameSet);
		const MappingType & mapping = getMapping();
		for(MappingType::const_iterator it=mapping.begin(); it!=mapping.end(); ++it)
		{
			tmp->insert(it->first);
		}
		refFrameSet = tmp;
	}
	return *refFrameSet;
}

const TRefFrameInfo::StringMappingType & TRefFrameInfo::getStringMapping()
{
	typedef std::auto_ptr<StringMappingType> StringMapPtr;
	static StringMapPtr stringMap;
	if(stringMap.get()==NULL)
	{
		StringMapPtr tmp = StringMapPtr(new StringMappingType);
		const MappingType & mapping = getMapping();
		for(MappingType::const_iterator it=mapping.begin(); it!=mapping.end(); ++it)
		{
			tmp->insert(std::make_pair(it->second.fRefFrameName, it->first));
		}
		stringMap = tmp;
	}
	return *stringMap;
}

bool TRefFrameInfo::isValidNumber(int frame)
{
	MappingType::const_iterator iter = getMapping().find(static_cast<TRefSystemFactory::ERefFrame>(frame));
	return iter!=getMapping().end();
}

TRefSystemFactory::ERefFrame TRefFrameInfo::fromNumber(int frame)
{
	if(isValidNumber(frame))
		return static_cast<TRefSystemFactory::ERefFrame>(frame);
	else
		throw std::invalid_argument("Unknown ERefFrame value");
}

std::string TRefFrameInfo::getEnumString(TRefSystemFactory::ERefFrame frame)
{
	MappingType::const_iterator iter = getMapping().find(static_cast<TRefSystemFactory::ERefFrame>(frame));
	if(iter!=getMapping().end())
	{
		return iter->second.fRefFrameName;
	}
	throw std::invalid_argument("Unknown ERefFrame value");
}

TRefSystemFactory::ERefFrame TRefFrameInfo::fromString(const std::string & s)
{
	StringMappingType::const_iterator it = getStringMapping().find(s);
	if(it==getStringMapping().end())
		throw std::invalid_argument("String does not match any known refframe");
	return it->second;
}

std::string TRefFrameInfo::getName(int frame)
{
	MappingType::const_iterator iter = getMapping().find(static_cast<TRefSystemFactory::ERefFrame>(frame));
	if(iter!=getMapping().end())
	{
		return iter->second.fName;
	}
	throw std::invalid_argument("Unknown ERefFrame value");
}

TCoordSysFactory::ECoordSys TRefFrameInfo::getDefaultCoordSys(int frame)
{
    MappingType::const_iterator iter = getMapping().find(static_cast<TRefSystemFactory::ERefFrame>(frame));
	if(iter!=getMapping().end())
	{
		return iter->second.fDefaultCoordSys;
	}
	throw std::invalid_argument("Unknown ERefFrame value");
}

bool TRefFrameInfo::isCoordSysAllowed(int frame, TCoordSysFactory::ECoordSys sys)
{
    MappingType::const_iterator iter = getMapping().find(static_cast<TRefSystemFactory::ERefFrame>(frame));
	if(iter!=getMapping().end())
	{
		return (iter->second.fAllowedCoordSys&sys);
	}
	throw std::invalid_argument("Unknown ERefFrame value");
}

int TRefFrameInfo::getAllowedCoordSysCount(int frame)
{
	MappingType::const_iterator iter = getMapping().find(static_cast<TRefSystemFactory::ERefFrame>(frame));
	if(iter!=getMapping().end())
	{
		int v = iter->second.fAllowedCoordSys;
		// count bits in an int:
		v = v - ((v >> 1) & 0x55555555);                    // reuse input as temporary
		v = (v & 0x33333333) + ((v >> 2) & 0x33333333);     // temp
		return ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24; // count
	}
	throw std::invalid_argument("Unknown ERefFrame value");
}

bool TRefFrameInfo::isLocalRefFrame(int frame)
{
    MappingType::const_iterator iter = getMapping().find(static_cast<TRefSystemFactory::ERefFrame>(frame));
    if(iter!=getMapping().end())
	{
		return iter->second.fLocal;
	}
	throw std::invalid_argument("Unknown ERefFrame value");
}

bool TRefFrameInfo::isRotatedLocalRefFrame(int frame) {
    MappingType::const_iterator iter = getMapping().find(static_cast<TRefSystemFactory::ERefFrame>(frame));
    if(iter!=getMapping().end())
	{
		// if the name starts with ML, it is a local modiefeid (i.e. rotated) frame
		return (iter->second.fRefFrameName.find("kML") == 0);
	}
	throw std::invalid_argument("Unknown ERefFrame value");}

TAReferenceFrame * TRefFrameInfo::getReferenceFrame(int frame, const TLocalSystemOrigin *lso)
{
	bool isdefinedlocal = false;
	TRefSystemFactory::EGeoid localgeoid = TRefSystemFactory::kNoGeoid;
    TRefSystemFactory::ERefFrame refFrame = fromNumber(frame);

	if (refFrame >= TRefSystemFactory::kMLA1985Machine && 
		refFrame <= TRefSystemFactory::kLGSphere) isdefinedlocal = true;

	if (isdefinedlocal && lso == NULL) 
		throw std::invalid_argument("LocalSystemOrigin required for the local reference frames!");
	else if (!isdefinedlocal && lso != NULL)
		throw std::invalid_argument("LocalSystemOrigin parameter is not needed for the non-*LA reference frames!");

	// determine the geoid of the predefined local frame
	switch (frame)
	{
		case TRefSystemFactory::kMLA1985Machine:
		case TRefSystemFactory::kLA1985Machine:
			localgeoid = TRefSystemFactory::kCG1985Machine;
			break;
		case TRefSystemFactory::kMLA2000Machine:
		case TRefSystemFactory::kLA2000Machine:
			localgeoid = TRefSystemFactory::kCG2000Machine;
			break;
		case TRefSystemFactory::kMLASphere:
		case TRefSystemFactory::kMLGSphere:
		case TRefSystemFactory::kLASphere:
		case TRefSystemFactory::kLGSphere:
			localgeoid = TRefSystemFactory::kCGSphere;
			break;
	}


	// return an appropriate instance
	if (isdefinedlocal)
		return TRefSystemFactory::getRefSystemFactory()->getNewLocalRefFrame(*lso, localgeoid, refFrame);
	else if(refFrame==TRefSystemFactory::kLocalRefFrame)
        return TRefSystemFactory::getRefSystemFactory()->getNewLocalRefFrame();
	else
		return TRefSystemFactory::getRefSystemFactory()->getRefFrame(refFrame);
}
