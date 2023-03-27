/*
© Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _LGCOPTIONS_H_
#define _LGCOPTIONS_H_

//SURVEYLIB
#include <TRefSystemFactory.h>
#include "TStatusObject.h"
//LGC
#include <Global.h>

#if USE_SERIALIZER
#	include <Serializer.hpp>
#endif // USE_SERIALIZER


/*!
	\ingroup LGCProjectData

	\brief This class contains the configuration options of the input file. 
	Options are set to defined default values by the starndard constructors of the
	related classes. 
*/
#if USE_SERIALIZER
	struct TLGCConfig : public Serializable
#else
	struct TLGCConfig
#endif // USE_SERIALIZER
{

	/*!
		Base class for options that are either einabled or disabled.
		The default is that the options are disabled this is done by 
		the default constructor.
	*/
	class TBinaryOption : public TStatusObject {
		
		public:
			/// Base constructor for binary options, disables an option by default.
			TBinaryOption(bool active = false) : TStatusObject(active) {}
			/// This virtual destructor does nothing.
			virtual ~TBinaryOption() {}

	};

	/*!
		Stores the options that are related to simulation
		input and output.
	*/

	class TSimulation : public TBinaryOption {
		public:
			/// The number of simulations to perform
			int  numSims;
			/// The seed number for the pseudo-random number generation/engine
			int  numSeed;
			/// write an output file that can be used as an imput file again
			bool writeLGCFile;

			/// The default constructor disables simulations. 
			// Disable simulation means that ignoreMeasurements should be, by default, set to FALSE not TRUE!
			TSimulation() :
				TBinaryOption(),
				numSims(0),
				numSeed(0),
				writeLGCFile(false) {}
			/*!
			\brief This constructor sets the default values for enabled simulations.

			\param n The number of simulations to run
			\param s The seed number
			\param ignoreMeas The measurement values in the input file are ignored
			*/
			TSimulation(int n, int s) :
				TBinaryOption(true),
				numSims(n),
				numSeed(s),
				writeLGCFile(false) {}

#if USE_SERIALIZER
				// Inherited via Serializable
				virtual void serialize(SerializerObject::SerializationHelper &obj) const override;
#endif
	};

	/*!
		Controls the blunder detection module.
	*/
	class TFautDetect : public TBinaryOption {
		public:

			TReal alpha, ///< alpha quantile for the fisher test
				  beta;  ///< beta quantile for the fisher test
			
			/// The default constructor disables the blunder detection
			TFautDetect()
                : TBinaryOption()
                , alpha(FAUT_DEF_ALPHA)
                , beta(FAUT_DEF_BETA) {}

			/*! 
				\brief Enables the blunder detection module with given quantiles.

				\param alpha_ left quantile for the test
				\param beta_ right quantile for the test
			*/
			TFautDetect(TReal alpha_ , TReal beta_) :
				TBinaryOption(true),
				alpha(alpha_),
				beta(beta_) {}

#if USE_SERIALIZER
				// Inherited via Serializable
				virtual void serialize(SerializerObject::SerializationHelper &obj) const override;
#endif
	};

	/*!
		Sets the orientation point for the adjustment
	*/
	class TPDOR : public TBinaryOption {
		public:
			/// Point name of the the orientation point
			std::string fptname;
			/// Constant gisement of the result
			TAngle fgis;
			//boolean to know if the bearing is given in the input file
			bool hasBearing; 
			
			/// There is no orientation point for the adjustment
			TPDOR()
                : TBinaryOption()
                , fptname("")
                , fgis(TAngle(0.0))
                , hasBearing(false) {}
			/*!
				\brief Adds an orientation point to the adjustment with an optional gisement.

				\param ptname The name of the orientation point as it is given in the points section
				\param gisement Constant gisement of the solution
			*/
			TPDOR(const std::string& ptname, TAngle gisement = TAngle(0.0)) :
				TBinaryOption(true),
				fptname(ptname),
				fgis(gisement),
				hasBearing(false){}

			#if USE_SERIALIZER
				// Inherited via Serializable
				virtual void serialize(SerializerObject::SerializationHelper &obj) const override;
			#endif
	};
	
	/// Defines the column separator for the output files.
	/// If no string is defined a fixed spaced output is used.
	class TCustomOutputSep : public TBinaryOption {
		public:
			/// Column separator string.
			std::string separator;
			
			/// A fixed spaced output is used by default.
			TCustomOutputSep() : TBinaryOption(), separator("") {}
			/// Sets a separator string, e.g. ';' or '\t'
			TCustomOutputSep(const std::string& sepstr) :
				TBinaryOption(true),
				separator(sepstr) {}

#if USE_SERIALIZER
			// Inherited via Serializable
			virtual void serialize(SerializerObject::SerializationHelper &obj) const override;
#endif
	};
	
	/// Specifies the coordinate output for deformation analysis or punc files.
	class TCoordOut : public TBinaryOption {
		public:
			/// Valid output modes
			enum eMode {
				kPLAIN, ///< Nom_Pt X Y Z
				kE,     ///< Nom_Pt X Y Z Vx Vy Vz Cxy Cxz Cyz Dx Dy Dz
				kEE,    ///< Nom_Pt X Y Z Gist_gd_axe Gd_axe Pt_axe Sz dx dy dz
				kH,     ///< Nom_Pt X Y H
				kZ,     ///< Nom_Pt X Y Z
				kHZ,    ///< Nom_Pt X Y Z H
				kHN,    ///< Nom_Pt X Y Z H NLEP
				kZHN,   ///< Nom_Pt X Y Z H NLEP
				kT,     ///< Nom_Pt X Y Z Sx Sy Sz
				kOUT1,  ///< .coo file for GEODE
				kOUT3,  ///< .coo file for GEODE
			};

			/// The desired output mode
			eMode fmode;
			
			/// The default behavior is to disable .coo files
			TCoordOut() : TBinaryOption(), fmode(kPLAIN) {}
			/// Enables vompensation results for the points in a given format
			TCoordOut(eMode mode) :
				TBinaryOption(true),
				fmode(mode) {}

#if USE_SERIALIZER
			// Inherited via Serializable
			virtual void serialize(SerializerObject::SerializationHelper &obj) const override;
#endif
	};

	/// Controls the precision of the output files
#if USE_SERIALIZER
	struct TPrecision : public Serializable
#else
	struct TPrecision
#endif // USE_SERIALIZER
	{
		public:
			/// Number of digits after the comma
			int digits;
			/// Convergence criterion based on the precision, see \ref calcConv .
			TReal convCrit;
			
			/// Sets the default precision, see \ref defaults.h
			TPrecision() :
				digits(PREC_DEF_DIGITS),
				convCrit(calcConv(digits)) {}
			TPrecision(int prec) :
				digits(prec),
				convCrit(calcConv(digits)) {}

			/// the convergence criterion is calculated by 5.0*pow(0.1, prec+1)
			// as it was done in previous LGC versions
			inline TReal calcConv(int prec) {
				return 5.0*powq(0.1, prec+1);
			}

#if USE_SERIALIZER
			// Inherited via Serializable
			virtual void serialize(SerializerObject::SerializationHelper &obj) const override;
#endif
	};

	/// The title of the adjustment including newlines
	std::string  title;
	/// See \ref Simulation
	TSimulation   sim;
	/// See \ref FautDetect
	TFautDetect   faut;
	/// See \ref TRefSystemFactory
	TRefSystemFactory::ERefFrame referential;
	
	/// Sets all points to be fixed points in spite of their configuration
	TBinaryOption allfixed;
	/// This is a free network adjustment
	TBinaryOption libre;
	/// duplicate measurements to a point are forbidden
	TBinaryOption nodup;
	/// See \ref PDOR
	TPDOR         pdor;
	
	/// See keyword APRI
	TBinaryOption useApriori;
	/// Creates ASCII-art histograms
	TBinaryOption histo;
	/// Enables error-ellipse data output
	TBinaryOption errorEllipses;
	/// Pairs of points for relative error analysis
	std::vector< std::pair<std::string, std::string> > erelPairs;
	/// See \ref CustomOutputSep
	TCustomOutputSep CustomOutputSeparator;
	/// See \ref CustomOutputSep
	TCustomOutputSep CustomOutputSeparatorPunch;
	/// See \ref Precision
	TPrecision	outPrecision;

	/// Write a file for deformation analysis
	TBinaryOption writeDefa;
	/// Write a file with adjustment results ,see \ref CoordOut for format details
	TCoordOut     writePunch;
	/// Write a file with adjustment results ,see \ref CoordOut for format details
	TCoordOut     writePlot;
	/// Write a file for covariance analysis
	TBinaryOption covar;
	/// Write a file for best-fit analysis 
	TBinaryOption chaba;
	/// Make a consistency check to find groups of unidentifiable objects
	TBinaryOption consCheck;
	/// Make a json serialized object
	TBinaryOption json;

	///width of point's name
	int pointNameWidth = 0;
	///width of observation's ID
	int obsIDwidth = 0;

	/*
	Checklist: 
		Referentiel defined? 
		if PDOR is used, is the given point defined?
		writePunch/ writePlot must not be set to kHN/kZHN when OLOC is used
		If a new LGC file should be written, SIM must be active

	*/
	void checkSanity();  //empty

#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(SerializerObject::SerializationHelper &obj) const override;
#endif // USE_SERIALIZER

};


#if USE_SERIALIZER
inline void TLGCConfig::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("allfixed", allfixed);
	obj.addProperty("chaba", chaba);
	obj.addProperty("consCheck", consCheck);
	obj.addProperty("covar", covar);
	obj.addProperty("CustomOutputSeparator", CustomOutputSeparator);
	obj.addProperty("CustomOutputSeparatorPunch", CustomOutputSeparatorPunch);

	//obj.addProperty("erelPairs", erelPairs);
	obj.addProperty("errorEllipses", errorEllipses);
	obj.addProperty("faut", faut);
	obj.addProperty("histo", histo);
	obj.addProperty("libre", libre);
	obj.addProperty("nodup", nodup);
	obj.addProperty("obsIDwidth", obsIDwidth);
	obj.addProperty("outPrecision", outPrecision);

	obj.addProperty("pdor", pdor);
	obj.addProperty("pointNameWidth", pointNameWidth);
	obj.addProperty("referential", referential);
	obj.addProperty("sim", sim);
	obj.addProperty("title", title);
	
	obj.addProperty("useApriori", useApriori);
	obj.addProperty("writeDefa", writeDefa);
	obj.addProperty("writePlot", writePlot);
	obj.addProperty("writePunch", writePunch);
	obj.addProperty("json", json);
}

inline void TLGCConfig::TSimulation::serialize(SerializerObject::SerializationHelper &obj) const
{
	TBinaryOption::serialize(obj);
	obj.addProperty("numSims", numSims);
	obj.addProperty("numSeed", numSeed);
	obj.addProperty("writeLGCFile", writeLGCFile);
}

inline void TLGCConfig::TFautDetect::serialize(SerializerObject::SerializationHelper &obj) const
{
	TBinaryOption::serialize(obj);
	obj.addProperty("alpha", alpha);
	obj.addProperty("beta", beta);
}

inline void TLGCConfig::TPDOR::serialize(SerializerObject::SerializationHelper &obj) const
{
	TBinaryOption::serialize(obj);
	obj.addProperty("fptname", fptname);
	obj.addProperty("fgis", fgis);
	obj.addProperty("hasBearing", hasBearing);
}

inline void TLGCConfig::TCustomOutputSep::serialize(SerializerObject::SerializationHelper &obj) const
{
	TBinaryOption::serialize(obj);
	obj.addProperty("separator", separator);
}

inline void TLGCConfig::TCoordOut::serialize(SerializerObject::SerializationHelper &obj) const
{
	TBinaryOption::serialize(obj);
	obj.addProperty("fmode", fmode);
}

inline void TLGCConfig::TPrecision::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("digits", digits);
	obj.addProperty("convCrit", convCrit);
}

#endif // USE_SERIALIZER

#endif 
