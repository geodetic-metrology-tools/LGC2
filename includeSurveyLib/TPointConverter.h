#ifndef SU_TPTCONVERTER
#define SU_TPTCONVERTER

#include "TAConverter.h"
#include "TRefSystemFactory.h"
#include "TLGCRefFrame.h"
#include "TAdjustablePoint.h"

/*!\ingroup Converters
 \brief Enables to write points in many different ways.
@{*/

//Class definition
class  TPointConverter : public TAConverter  
{
public:

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
/*			TPointConverter(TAStreamFormatter* stream,
							const TRefSystemFactory::ERefFrame	fRefFrame);*/

			TPointConverter(TAStreamFormatter* stream,
							const TLGCRefFrame::ERefs	fRefFrame);


			//!Destructor
			virtual  ~TPointConverter();
		//@}

		//! copy assignment operator
		TPointConverter& operator=(const TPointConverter& source);



	/*!@name Public member functions*/
		//@{

//			void	writeName(LSPosVecConstIter pt, int width);

			void	writeName(string ptName, int width);

			void	writeXYZandH(const TAdjustablePoint& pt);

			void	writeXYZandH(const TPositionVector& pt);
			void	writeXYZandH(const TPositionVector& pt, TReal heightH);


			void	writeXYZandH(	const int width,
									const int precision,
									const TLength::EUnits unit,
									const string separator,
									TAdjustablePoint pt);


/*			void	writeXYZorH(	const int width,
									const int precision,
									const TLength::EUnits unit,
									const string separator,
									LSPosVecConstIter pt);
*/
			void	writeXYH(	const int width,
								const int precision,
								const TLength::EUnits unit,
								const string separator,
								TAdjustablePoint pt);

/*			void	writeXYZ(	const int width,
								const int precision,
								const TLength::EUnits unit,
								const string separator,
								LSPosVecConstIter pt);
*/
			void	writeXYZ(	const int width,
								const int precision,
								const TLength::EUnits unit,
								const string separator,
								const TPositionVector pt);

			void	write3Coordinates(	const int width,
										const int precision,
										const string separator,
										const TPositionVector pt);


			void	writeN( const int width,
							const int precision,
							const TLength::EUnits unit,
							TAdjustablePoint pt);


			void	writeCoordinateParam(	const TSpatialStatus::ESpatialStatus status,
											const int width,
											const int precision,
											const TLength::EUnits unit,
											const string separator,
											const TLength Xparam,
											const TLength Yparam,
											const TLength Zparam,
											const string Dparam,
											bool isCovar = false);


			void	writeCoordinateParam(	const TSpatialStatus::ESpatialStatus status,
											const int width,
											const int precision,
											const string separator,
											const TScalar Xparam,
											const TScalar Yparam,
											const TScalar Zparam,
											const string Dparam,
											bool isCovar = false);

	
			void	writeCoordinateParam(	const TSpatialStatus::ESpatialStatus status,
											const int width,
											const int precision,
											const string separator,
											const TDouble Xparam,
											const TDouble Yparam,
											const TDouble Zparam,
											const string Dparam);

			bool	isInLocalSystem() const {return	fLocalSys;}

		//@}


private:

			//!constructor
			TPointConverter();


private:


		bool							fLocalSys;

		TLGCRefFrame::ERefs	fRefFrame;
};

/*@}*/

#endif // SU_TPTCONVERTER
