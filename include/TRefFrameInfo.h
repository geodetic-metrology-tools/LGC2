#ifndef CSGEOCORE_T_REF_FRAME_INFO_H_78d85635badc4fa08dd883eb60f912d4
#define CSGEOCORE_T_REF_FRAME_INFO_H_78d85635badc4fa08dd883eb60f912d4

#include <TCoordSysFactory.h>
#include <TRefSystemFactory.h>
#include <map>
#include <set>


/**
 *  Class for providing additional related information to the TRefSystemFactory::ERefFrame.
 *
 *  Each reference frame is being identified by a value from ERefFrame enum.
 *  However there is a need to associate some additional information
 *  with the enum values. For example:
 *     - Coordinate system  
 *     - User friendly name
 *
 *  In addition the class provides an easy way to iterate over the all 
 *  supported ERefFrame enum values.
 */
class TRefFrameInfo
{
private:
	/*! Constructor is private and has no definition. This class cannot be instantiated. */
	TRefFrameInfo() {}
	TRefFrameInfo(const TRefFrameInfo&) {} // non construction-copyable
	const TRefFrameInfo & operator=(const TRefFrameInfo &); // non copyable

	struct TDetails
	{
        /**
         *  Initialise the Reference Frame information structure.
		 *  \param[in] reference frame enum name
         *  \param[in] name user friendly name (for GUI, help, etc) of the refFrame
         *  \param[in] defaultCoordSys Default coordinate system for the refFrame.
         *               It is automatically added to the list of allowed coordinate systems.
         *  \param[in] allowedCoordSys optional parameter for specifying additional 
         *               permitted coordinate systems. Default value 0 means that no additional
         *               coordinate systems are allowed.
         *  \param[in] local optional parameter for specifying whether we are dealing with
         *               a local reference frame (e.g. MLA)
         */
		TDetails(
			const std::string & refFrameName,
			const std::string & name, 
			TCoordSysFactory::ECoordSys defaultCoordSys,
			int allowedCoordSys=0, 
			bool local=false);

		const std::string fRefFrameName;
		const std::string fName;
		const TCoordSysFactory::ECoordSys fDefaultCoordSys;
        const int fAllowedCoordSys;
        const bool fLocal;

		private:
			TDetails & operator=(const TDetails &);  // non copyable
	};
	typedef std::map<TRefSystemFactory::ERefFrame, TDetails> MappingType;
    /**
     * We cannot have a static map member, due to the risk of static initialisation fiasco:
     * http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.12
     */
	static const MappingType & getMapping();

	typedef std::map<std::string, TRefSystemFactory::ERefFrame> StringMappingType;
	static const StringMappingType & getStringMapping();
public:
	typedef std::set<TRefSystemFactory::ERefFrame> RefFrameSet;

	/**
	 *  \return The set containing all the supported ERefFrame enum values.
	 */
	static const RefFrameSet & getRefFrameSet();

	/**
	 *  Check if the value can be converted into a registered
	 *  TRefSystemFactory::ERefFrame value.
     *
	 *  \param[in] frame Integer value corresponding to the ERefFrame enum value
	 */
	static bool isValidNumber(int frame);

	/**
	 *  Convert the value a registered TRefSystemFactory::ERefFrame value.
     *
	 *  \param[in] frame Integer value corresponding to the ERefFrame enum value
	 *	\return Corresponding TRefSystemFactory::ERefFrame value
	 *  \throw std::invalid_argument If the input parameter is not valid
	 */
	static TRefSystemFactory::ERefFrame fromNumber(int frame);
	static TRefSystemFactory::ERefFrame fromString(const std::string & s);
	static std::string getEnumString(TRefSystemFactory::ERefFrame frame);

	/**
	 *  Retrieve a user friendly name of the ERefFrame.
	 *
	 *  \param[in] frame Integer value corresponding to the ERefFrame enum value
	 *  \return A user friendly name associated with the given reference frame
	 *  \throw std::invalid_argument If the input parameter is not valid
	 */
	static std::string getName(int frame);

	/**
	 *  Retrieve a default coordinate system for the ERefFrame.
	 *
	 *  \param[in] frame Integer value corresponding to the ERefFrame enum value
	 *  \return Default TCoordSysFactory::ECoordSys value for the given reference frame
	 *  \throw std::invalid_argument If the input parameter is not valid
	 */
	static TCoordSysFactory::ECoordSys getDefaultCoordSys(int frame);

    static bool isCoordSysAllowed(int frame, TCoordSysFactory::ECoordSys sys);
	static int getAllowedCoordSysCount(int frame);
	
    static bool isLocalRefFrame(int frame);
    static bool isRotatedLocalRefFrame(int frame);

    static TAReferenceFrame * getReferenceFrame(int frame, const TLocalSystemOrigin * lso = NULL);
};


#endif
