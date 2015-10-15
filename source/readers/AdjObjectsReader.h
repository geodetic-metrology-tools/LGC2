#ifndef _READERS_ADJOBJECTSREADER_H_
#define _READERS_ADJOBJECTSREADER_H_


#include "ReaderBase.h"

class TKeyFRAME : public TAKeyWord {
	public:
		/*!
			The frame parser takes the lines of frame definitions and creates the necessary 
			local transformations to jump from one frame to another. The frame-of-frames-tree must be
			constructed on the side that uses this object, accordingly the collection of frames
			is a reference to a pointer so that the same object can be used on all levels of the tree.
		*/
		TKeyFRAME(TLGCData& project);

		/*!		
			Parse line with the frame definition.
		*/
		virtual void parse(const std::vector<const std::string>& tokens, int line);

		///Returns the number of frames that were created
		static int getNumberOfOpenedFrames() {return fNumberOfFramesOpened;};

	private:
		TKeyFRAME& operator=(const TKeyFRAME&);
		///counter of number of frames introduced, to chceck if all *FRAMES were properly closed
		static int fNumberOfFramesOpened; 	
};


class TKeyENDFRAME : public TAKeyWord {
	public:
		/*!
		Counterpart of *FRAME, closes the current frame definition

		*/
		TKeyENDFRAME(TLGCData& project);
				
		/*!
			Endframe does not parse anything, just a marker to move up the tree again and report.
			\throws runtime_error if arguments in *ENDFRAME line (there should be nothing else)
		*/
		virtual void parse(const std::vector<const std::string>& tokens, int);

		///Returns the number of frames that were closed
		static int getNumberOfClosedFrames() {return fNumberOfFramesClosed;};
		
	private:
		static int fNumberOfFramesClosed; 
};



class TKeyPoint : public TAKeyWord {
	public:
		TKeyPoint(TLGCData& project, const std::string& word);
		
		size_t findComment(const std::string& s);

		virtual void parse(const std::vector<const std::string>& tokens, int line);

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
		virtual TAdjustablePoint& insertPoint(const std::string& ID, TReal x, TReal y, TReal z) = 0;

		TKeyPoint& operator=(const TKeyPoint&);
};

class TKeyCALA : public TKeyPoint {
	public:
		TKeyCALA(TLGCData& project);
				
	protected:		 
		virtual TAdjustablePoint& insertPoint(const std::string& pointName, TReal x, TReal y, TReal z);
};

class TKeyPOIN : public TKeyPoint {
	public:
		TKeyPOIN(TLGCData& project);				
	protected:
		virtual TAdjustablePoint& insertPoint(const std::string& pointName, TReal x, TReal y, TReal z);
};

class TKeyVXY : public TKeyPoint {
	public:
		TKeyVXY(TLGCData& project);
	protected:
		virtual TAdjustablePoint& insertPoint(const std::string& pointName, TReal x, TReal y, TReal z);
};

class TKeyVXZ : public TKeyPoint {
	public:
		TKeyVXZ(TLGCData& project);

	protected:
		virtual TAdjustablePoint& insertPoint(const std::string& pointName, TReal x, TReal y, TReal z);
};

class TKeyVYZ : public TKeyPoint {
	public:
		TKeyVYZ(TLGCData& project);
		
	protected:
		virtual TAdjustablePoint& insertPoint(const std::string& pointName, TReal x, TReal y, TReal z);
};

class TKeyVZ : public TKeyPoint {
	public:
		TKeyVZ(TLGCData& project);
		
	protected:
		virtual TAdjustablePoint& insertPoint(const std::string& pointName, TReal x, TReal y, TReal z);
};

#endif
