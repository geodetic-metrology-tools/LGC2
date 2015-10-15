#import <swisstopoReframeLib.tlb>
#include <TCoInitializer.h>
#include <TLV03Transformation.h>
#include <TRefFrameInfo.h>

#include <iostream>
#include <sstream>


namespace compute_gps_ref_modes {
	enum compute_gps_ref_modes
	{
		GLOBAL_GEOCENTRIC_TO_SWISS_PLANE = 0,
		GLOBAL_GEOGRAPHIC_TO_SWISS_PLANE = 1,
		SWISS_PLANE_TO_GLOBAL_GEOCENTRIC = 2,
		SWISS_PLANE_TO_GLOBAL_GEOGRAPHIC = 3
	};
}

namespace compute_reframe_plane {
    enum compute_reframe_plane
    {
        LV03 = 0,
        LV95 = 1
    };
}

namespace compute_reframe_alt {
    enum compute_reframe_alt
    {
        LN02 = 0,
        LHN95 = 1, // Requires additional library
        BESSEL = 2
    };
}

TLV03Transformation::TLV03Transformation(bool fFromLV95)
: fFromLV95(fFromLV95)
{
}

TLV03Transformation * TLV03Transformation::clone() const
{
    return new TLV03Transformation(*this);
}

TLV03Transformation * TLV03Transformation::inverse() const
{
    return new TLV03Transformation(!fFromLV95);
}

TAReferenceFrame * TLV03Transformation::getSourceFrame() const
{
    return TRefFrameInfo::getReferenceFrame(
        fFromLV95 ? TRefSystemFactory::kSwissLV95 : TRefSystemFactory::kSwissLV03);
}

TAReferenceFrame * TLV03Transformation::getDestinationFrame() const
{
    return TRefFrameInfo::getReferenceFrame(
        fFromLV95 ? TRefSystemFactory::kSwissLV03 : TRefSystemFactory::kSwissLV95);
}

bool TLV03Transformation::transform(TPositionVector & pv) const
{
    TSpatialPosition position(getSourceFrame());
	if(!position.setCoordinates(pv))
        return false;

    double x = position.getCoordinates(TCoordSysFactory::k2DPlusH).getX().getMetresValue();
    double y = position.getCoordinates(TCoordSysFactory::k2DPlusH).getY().getMetresValue();
    double h = position.getCoordinates(TCoordSysFactory::k2DPlusH).getH().getMetresValue();

    bool result = true;
    if(fFromLV95)
        result = computeSwissRefFrame(x, y, h, compute_reframe_plane::LV95, compute_reframe_plane::LV03);
    else
        result = computeSwissRefFrame(x, y, h, compute_reframe_plane::LV03, compute_reframe_plane::LV95);

    if(!result)
        return false;

    pv.setX(TLength(x));
    pv.setY(TLength(y));
    pv.setH(TLength(h));

    return true;
}

bool TLV03Transformation::computeSwissRefFrame(double & coordinate_x, double & coordinate_y, double & coordinate_z, int reframe_in, int reframe_out) const
{
    int result = 1;
    try
    {
		CoInitialize(NULL);
        //CCoInitializer coinit(COINIT_MULTITHREADED);
        swisstopoReframeLib::IReframePtr pReframe(__uuidof(swisstopoReframeLib::Reframe));
        pReframe->SetDatasetsDir("C:/Program Files (x86)/swisstopo/ReframeDLL/datasets");

	    // Transform LV95 coordinates to LV03 and Bessel height to Bessel
        result = pReframe->ComputeReframe(
            &coordinate_y, &coordinate_x, &coordinate_z, 
            reframe_in,reframe_out,
            compute_reframe_alt::BESSEL,
            compute_reframe_alt::BESSEL);
    }
	catch (const _com_error & e)
	{
		std::stringstream s;
		s << "Reframe DLL unavailable, detailed error: " << e.ErrorMessage();
		std::cerr << s.str() << std::endl;
		throw std::logic_error(s.str());
	}
    catch (HRESULT&)
    {
        std::cerr << "Reframe DLL - CoInitialize failed. DLL is not registered" << std::endl;
		throw std::logic_error("Reframe DLL - CoInitialize failed. DLL is not registered");
	    return false;
    }
	if (result==1)
	{
        return true;
	}
	else if (result== -1)
	{
		std::cerr<<"Error: 1 specified point outside of the CHENyx06 triangular network (input coordinates outside boundaries)"<<std::endl;
		throw std::logic_error("Error: 1 specified point outside of the CHENyx06 triangular network (input coordinates outside boundaries)");
	}
	else if (result== -2)
	{
		std::cerr<<"Error: 2 specified point outside of the HTRANS or CHGeo2004 grid (input coordinates outside boundaries)"<<std::endl;
		throw std::logic_error("Error: 2 specified point outside of the HTRANS or CHGeo2004 grid (input coordinates outside boundaries)");
	}
	else if (result== -3)
	{
		std::cerr<<"Error: 3 problem occurred when reading a binary file. Check that all the binary files(datasets definitions) are correctly installed and valid (try to recover/recopy the original"
					"versions). Reinstall the REFRAME DLL if the problem persists."<<std::endl;
		throw std::logic_error("Error: 3 problem occurred when reading a binary file. Check that all the binary files(datasets definitions) are correctly installed and valid (try to recover/recopy the original"
					"versions). Reinstall the REFRAME DLL if the problem persists.");
	}
	
	else if (result== -4)
	{
		std::cerr<<"Error: 4 unsupported value for \"plaFrameIn\" or \"plaFrameOut\" argument (only \"0\" or \"1\" are allowed)"<<std::endl;
		throw std::logic_error("Error: 4 unsupported value for \"plaFrameIn\" or \"plaFrameOut\" argument (only \"0\" or \"1\" are allowed)");
	}
	
	else if (result== -5)
	{
		std::cerr<<"Error: 5 unsupported value for \"altFrameIn\" or \"altFrameOut\" argument (only \"0\", \"1\" or \"2\"7 are allowed)"<<std::endl;
		throw std::logic_error("Error: 5 unsupported value for \"altFrameIn\" or \"altFrameOut\" argument (only \"0\", \"1\" or \"2\"7 are allowed)");
	}
	
	else if (result== -6)
	{
		std::cerr<<"Error: 6 input and output reference frames (planimetry and altimetry) are the same, there isn’t any transformation to do!"<<std::endl;
		throw std::logic_error("Error: 6 input and output reference frames (planimetry and altimetry) are the same, there isn’t any transformation to do!");
	}
	
	else if (result== -10)
	{
		std::cerr<<"Error: 10 Check if the path defined in the \"datasetsDir\" property (or with the \"SetDatasetsDir\" method) is correct and that the file is accessible (enough rights)."<<std::endl;
		throw std::logic_error("Error: 10 Check if the path defined in the \"datasetsDir\" property (or with the \"SetDatasetsDir\" method) is correct and that the file is accessible (enough rights).");
	}
	
	else if (result== -11)
	{
		std::cerr<<" Error: 11 Check if the path defined in the \"binaryDataDir\" is correct and thatthe file is accessible (enough rights)."<<std::endl;
		throw std::logic_error(" Error: 11 Check if the path defined in the \"binaryDataDir\" is correct and thatthe file is accessible (enough rights).");
	}
	
	else if (result== -12)
	{
		std::cerr<<"Error: 12 CHGeo2004 dataset is inaccessible (binary file \"CHGEO04.grd\" not found)."<<std::endl;
		throw std::logic_error("Error: 12 CHGeo2004 dataset is inaccessible (binary file \"CHGEO04.grd\" not found).");
	}
	else
	{
		std::cerr<<"Error: unknown error code: " << result << std::endl;
		throw std::logic_error("Error: unknown error code: ");
	}

}

