#ifndef _READERS_ADJOBJECTSREADER_H_
#define _READERS_ADJOBJECTSREADER_H_

#include "ReaderBase.h"
#include "KeywordRights.h"

/*!
 *  \addtogroup AdjObjectsReader
 *  @{
 */
class TKeyFRAME : public TAKeyWord {
	public:
		/*!
			The frame parser takes the lines of frame definitions and creates the necessary 
			local transformations to jump from one frame to another. The frame-of-frames-tree must be
			constructed on the side that uses this object, accordingly the collection of frames
			is a reference to a pointer so that the same object can be used on all levels of the tree.
		*/
		TKeyFRAME(TLGCData& project, int nb_allowed_keywords = nb_allowed_frame, const char** keywords = allowed_FRAME);

	
		///	Parse line with the frame definition.
		virtual void parse(const std::vector<std::string>& tokens, int line);

		///Returns the number of frames that were created
		static int getNumberOfOpenedFrames() {return fNumberOfFramesOpened;};

	private:
		TKeyFRAME& operator=(const TKeyFRAME&);

		///counter of number of frames introduced, to chceck if all *FRAMES were properly closed
		static int fNumberOfFramesOpened; 	
};


class TKeyENDFRAME : public TAKeyWord {
	public:

		///	Counterpart of *FRAME, closes the current frame definition
		TKeyENDFRAME(TLGCData& project, int nb_allowed_keywords = nb_allowed_ef, const char** keywords = allowed_ENDFRAME);
				
		/*!
			\brief Endframe does not parse anything, just a marker to move up the tree again and report.

			\throws runtime_error if arguments in *ENDFRAME line (there should be nothing else)
		*/
		virtual void parse(const std::vector<std::string>& tokens, int);

		///Returns the number of frames that were closed
		static int getNumberOfClosedFrames() {return fNumberOfFramesClosed;};
		
	private:
		static int fNumberOfFramesClosed; 
};


class TAPointKey : public TAKeyWord {
	public:
		TAPointKey(TLGCData& project, const std::string& word);
		
		size_t findComment(const std::string& s);

		virtual void parse(const std::vector<std::string>& tokens, int line);

	protected:
		// standard error message if a point is not specified correctly
		const std::string defaultErrmsg;
		// read-only access to configuration
		const TLGCConfig& fconfig;
		// access to point storage
		TAdjustablePointCollection& fpointAccess;
		// comments before and in the current line
		std::string hdrcomment;

		// Template pattern: insert the correct point object into the collection by subclass implementation
		virtual LGCAdjustablePoint& insertPoint(const std::string& ID, TReal x, TReal y, TReal z) = 0;

		TAPointKey& operator=(const TAPointKey&);
};

class TKeyCALA : public TAPointKey {
	public:
		TKeyCALA(TLGCData& project, int nb_allowed_keywords = nb_allowed_cala, const char** keywords = allowed_CALA);
	protected:		 
		virtual LGCAdjustablePoint& insertPoint(const std::string& pointName, TReal x, TReal y, TReal z);
};

class TKeyPOIN : public TAPointKey {
	public:
		TKeyPOIN(TLGCData& project, int nb_allowed_keywords = nb_allowed_poin, const char** keywords = allowed_POIN);				
	protected:
		virtual LGCAdjustablePoint& insertPoint(const std::string& pointName, TReal x, TReal y, TReal z);
};

class TKeyVXY : public TAPointKey {
	public:
		TKeyVXY(TLGCData& project, int nb_allowed_keywords = nb_allowed_vxy, const char** keywords = allowed_VXY);
	protected:
		virtual LGCAdjustablePoint& insertPoint(const std::string& pointName, TReal x, TReal y, TReal z);
};

class TKeyVXZ : public TAPointKey {
	public:
		TKeyVXZ(TLGCData& project, int nb_allowed_keywords = nb_allowed_vxz, const char** keywords = allowed_VXZ);

	protected:
		virtual LGCAdjustablePoint& insertPoint(const std::string& pointName, TReal x, TReal y, TReal z);
};

class TKeyVYZ : public TAPointKey {
	public:
		TKeyVYZ(TLGCData& project, int nb_allowed_keywords = nb_allowed_vyz, const char** keywords = allowed_VYZ);
		
	protected:
		virtual LGCAdjustablePoint& insertPoint(const std::string& pointName, TReal x, TReal y, TReal z);
};

class TKeyVZ : public TAPointKey {
	public:
		TKeyVZ(TLGCData& project, int nb_allowed_keywords = nb_allowed_vz, const char** keywords = allowed_VZ);
		
	protected:
		virtual LGCAdjustablePoint& insertPoint(const std::string& pointName, TReal x, TReal y, TReal z);
};
/*! @} End of Doxygen Groups*/

#endif
