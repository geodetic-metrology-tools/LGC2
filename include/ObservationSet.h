// ObservationSet.h


#ifndef SU_OBS_SET
#define SU_OBS_SET

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "TWorkingObservation.h"
#include "THorAngleObservation.h"
#include "TSpatialDistObservation.h"
#include "TZenithDistObservation.h"
#include "THorizontalDistObservation.h"
#include "TVertDistObservation.h"
#include "TOffsetToVerLineObservation.h"
#include "TOffsetToSpaLineObservation.h"
#include "TOffsetToVerPlaneObservation.h"
#include "TOffsetToTheoPlaneObservation.h"
#include "TGyroOrientationObservation.h"
#include "TRadialOffsetCnstrObservation.h"
#include "TOrientationCnstrObservation.h"

class ObservationSet {

public:

	/*!@name Constructor / destructor */
	//@{
	/*! Default constructor */
	ObservationSet();
	/*! Destructor */
	~ObservationSet();
	//@}

	/*!@name Access to hor.ang. observations list's items */
	//@{
		/*! Returns an iterator to the first item of the list */
		HorAngObsIter		beginHorAng();
		/*! Returns a const iterator to the first item of the list */
		HorAngObsConstIter beginHorAng() const;
		/*! Returns an iterator to the position after the last item of the list */
		HorAngObsIter		endHorAng();
		/*! Returns a const iterator to the position after the last item of the list */
		HorAngObsConstIter endHorAng() const;
		/*! Adds a new item to the list and returns an iterator */
		void		push_backHorAng(const THorAngleObservation& ha, bool noDuplicates);
		/*!\return the list's size*/
		int					sizeHorAng() const {return fWorkingHorAngObs->size();}
		bool				getWorkingHorAngObsError() const { return fWorkingHorAngObs->getListError(); }
	//@}

	/*!@name Access to zenith dist. observations list's items */
	//@{
		/*! Returns an iterator to the first item of the list */
		ZenDistObsIter		beginZenDist();
		/*! Returns a const iterator to the first item of the list */
		ZenDistObsConstIter	beginZenDist() const;
		/*! Returns an iterator to the position after the last item of the list */
		ZenDistObsIter		endZenDist();
		/*! Returns a const iterator to the position after the last item of the list */
		ZenDistObsConstIter	endZenDist() const;
		/*! Adds a new item to the list and returns an iterator */
		void		push_backZenDist(const TZenithDistObservation& zd, bool noDuplicates);
		/*!\return the list's size*/
		int					sizeZenDist() const {return fWorkingZenDistObs->size();}
		bool				getWorkingZenDistObsError() const { return fWorkingZenDistObs->getListError(); }
	//@}

	/*!@name Access to spatial dist. observations list's items */
	//@{
		/*! Returns an iterator to the first item of the list */
		SpaDistObsIter		beginSpaDist();
		/*! Returns a const iterator to the first item of the list */
		SpaDistObsConstIter	beginSpaDist() const;
		/*! Returns an iterator to the position after the last item of the list */
		SpaDistObsIter		endSpaDist();
		/*! Returns a const iterator to the position after the last item of the list */
		SpaDistObsConstIter	endSpaDist() const;
		/*! Adds a new item to the list and returns an iterator */
		void		push_backSpaDist(const TSpatialDistObservation& sd, bool noDuplicates);
		/*!\return the list's size*/
		int					sizeSpaDist() const {return fWorkingSpaDistObs->size();}
		bool				getWorkingSpaDistObsError() const { return fWorkingSpaDistObs->getListError(); }
	//@}

	/*!@name Access to horizontal dist. observations list's items */
	//@{
		/*! Returns an iterator to the first item of the list */
		HorDistObsIter		beginHorDist();
		/*! Returns a const iterator to the first item of the list */
		HorDistObsConstIter	beginHorDist() const;
		/*! Returns an iterator to the position after the last item of the list */
		HorDistObsIter		endHorDist();
		/*! Returns a const iterator to the position after the last item of the list */
		HorDistObsConstIter	endHorDist() const;
		/*! Adds a new item to the list and returns an iterator */
		void		push_backHorDist(const THorizontalDistObservation& hd, bool noDuplicates);
		/*!\return the list's size*/
		int					sizeHorDist() const {return fWorkingHorDistObs->size();}
		bool				getWorkingHorDistObsError() const { return fWorkingHorDistObs->getListError(); }
	//@}

	/*!@name Access to vertical dist. observations list's items */
	//@{
		/*! Returns an iterator to the first item of the list */
		VertDistObsIter			beginVertDist(bool isDLEV);
		/*! Returns a const iterator to the first item of the list */
		VertDistObsConstIter	beginVertDist(bool isDLEV) const;
		/*! Returns an iterator to the position after the last item of the list */
		VertDistObsIter			endVertDist(bool isDLEV);
		/*! Returns a const iterator to the position after the last item of the list */
		VertDistObsConstIter	endVertDist(bool isDLEV) const;
		/*! Adds a new item to the list and returns an iterator */
		void			push_backVertDist(const TVertDistObservation& vd, bool noDuplicates, bool isDLEV);
		/*!\return the list's size*/
		int						sizeVertDist(bool isDLEV) const;
		bool					getWorkingVertDistObsError(bool isDLEV) const;
	//@}

	/*!@name Access to offset to Ver. Line observations list's items */
	//@{
		/*! Returns an iterator to the first item of the list */
		OffsetToVerLineObsIter		beginOffsetToVerLine();
		/*! Returns a const iterator to the first item of the list */
		OffsetToVerLineObsConstIter	beginOffsetToVerLine() const;
		/*! Returns an iterator to the position after the last item of the list */
		OffsetToVerLineObsIter		endOffsetToVerLine();
		/*! Returns a const iterator to the position after the last item of the list */
		OffsetToVerLineObsConstIter	endOffsetToVerLine() const;
		/*! Adds a new item to the list and returns an iterator */
		void		push_backOffsetToVerLine(const TOffsetToVerLineObservation& vd, bool noDuplicates);
		/*!\return the list's size*/
		int							sizeOffsetToVerLine() const {return fWorkingOffsetToVerLineObs->size();}
		bool						getWorkingOffsetToVerLineObsError() const { return fWorkingOffsetToVerLineObs->getListError(); }
	//@}

	/*!@name Access to offset to Spa. Line observations list's items */
	//@{
		/*! Returns an iterator to the first item of the list */
		OffsetToSpaLineObsIter		beginOffsetToSpaLine();
		/*! Returns a const iterator to the first item of the list */
		OffsetToSpaLineObsConstIter	beginOffsetToSpaLine() const;
		/*! Returns an iterator to the position after the last item of the list */
		OffsetToSpaLineObsIter		endOffsetToSpaLine();
		/*! Returns a const iterator to the position after the last item of the list */
		OffsetToSpaLineObsConstIter	endOffsetToSpaLine() const;
		/*! Adds a new item to the list and returns an iterator */
		void		push_backOffsetToSpaLine(const TOffsetToSpaLineObservation& vd, bool noDuplicates);
		/*!\return the list's size*/
		int							sizeOffsetToSpaLine() const {return fWorkingOffsetToSpaLineObs->size();}
		bool						getWorkingOffsetToSpaLineObsError() const { return fWorkingOffsetToSpaLineObs->getListError(); }
	//@}

	/*!@name Access to offset to Ver. Plane observations list's items */
	//@{
		/*! Returns an iterator to the first item of the list */
		OffsetToVerPlaneObsIter			beginOffsetToVerPlane();
		/*! Returns a const iterator to the first item of the list */
		OffsetToVerPlaneObsConstIter	beginOffsetToVerPlane() const;
		/*! Returns an iterator to the position after the last item of the list */
		OffsetToVerPlaneObsIter			endOffsetToVerPlane();
		/*! Returns a const iterator to the position after the last item of the list */
		OffsetToVerPlaneObsConstIter	endOffsetToVerPlane() const;
		/*! Adds a new item to the list and returns an iterator */
		void			push_backOffsetToVerPlane(const TOffsetToVerPlaneObservation& vd, bool noDuplicates);
		/*!\return the list's size*/
		int								sizeOffsetToVerPlane() const {return fWorkingOffsetToVerPlaneObs->size();}
		bool							getWorkingOffsetToVerPlaneObsError() const { return fWorkingOffsetToVerPlaneObs->getListError(); }
	//@}

	/*!@name Access to offset to Theo. Plane observations list's items */
	//@{
		/*! Returns an iterator to the first item of the list */
		OffsetToTheoPlaneObsIter		beginOffsetToTheoPlane();
		/*! Returns a const iterator to the first item of the list */
		OffsetToTheoPlaneObsConstIter	beginOffsetToTheoPlane() const;
		/*! Returns an iterator to the position after the last item of the list */
		OffsetToTheoPlaneObsIter		endOffsetToTheoPlane();
		/*! Returns a const iterator to the position after the last item of the list */
		OffsetToTheoPlaneObsConstIter	endOffsetToTheoPlane() const;
		/*! Adds a new item to the list and returns an iterator */
		void		push_backOffsetToTheoPlane(const TOffsetToTheoPlaneObservation& vd, bool noDuplicates);
		/*!\return the list's size*/
		int								sizeOffsetToTheoPlane() const {return fWorkingOffsetToTheoPlaneObs->size();}
		bool							getWorkingOffsetToTheoPlaneObsError() const { return fWorkingOffsetToTheoPlaneObs->getListError(); }
	//@}


	/*!@name Access to orie. observations list's items */
	//@{
		/*! Returns an iterator to the first item of the list */
		GyroOrieObsIter		beginGyroOrie();
		/*! Returns a const iterator to the first item of the list */
		GyroOrieObsConstIter beginGyroOrie() const;
		/*! Returns an iterator to the position after the last item of the list */
		GyroOrieObsIter		endGyroOrie();
		/*! Returns a const iterator to the position after the last item of the list */
		GyroOrieObsConstIter endGyroOrie() const;
		/*! Adds a new item to the list and returns an iterator */
		void		push_backGyroOrie(const TGyroOrientationObservation& obs, bool noDuplicates);
		/*!\return the list's size*/
		int					sizeGyroOrie() const {return fWorkingGyroOrieObs->size();}
		bool				getWorkingGyroOrieObsError() const { return fWorkingGyroOrieObs->getListError(); }
	//@}

	/*!@name Access to radial offset constraint observations list's items */
	//@{
		/*! Returns an iterator to the first item of the list */
		RadOffCnstrObsIter		beginRadOffCnstr();
		/*! Returns a const iterator to the first item of the list */
		RadOffCnstrObsConstIter beginRadOffCnstr() const;
		/*! Returns an iterator to the position after the last item of the list */
		RadOffCnstrObsIter		endRadOffCnstr();
		/*! Returns a const iterator to the position after the last item of the list */
		RadOffCnstrObsConstIter endRadOffCnstr() const;
		/*! Adds a new item to the list and returns an iterator */
		void					push_backRadOffCnstr(const TRadialOffsetCnstrObservation& obs, bool noDuplicates);
		/*!\return the list's size*/
		int						sizeRaddOffCnstr() const {return fWorkingRadOffCnstrObs->size();}
		bool					getWorkingRadOffCnstrError() const { return fWorkingRadOffCnstrObs->getListError(); }
	//@}

	/*!@name Access to orientation constraint observations list's items */
	//@{
		/*! Returns an iterator to the first item of the list */
		OrieCnstrObsIter		beginOrieCnstr();
		/*! Returns a const iterator to the first item of the list */
		OrieCnstrObsConstIter	beginOrieCnstr() const;
		/*! Returns an iterator to the position after the last item of the list */
		OrieCnstrObsIter		endOrieCnstr();
		/*! Returns a const iterator to the position after the last item of the list */
		OrieCnstrObsConstIter	endOrieCnstr() const;
		/*! Adds a new item to the list and returns an iterator */
		void		push_backOrieCnstr(const TOrientationCnstrObservation& obs, bool noDuplicates);
		/*!\return the list's size*/
		int						sizeOrieCnstr() const {return fWorkingOrieCnstrObs->size();}
		bool					getWorkingOrieCnstrObsError() const { return fWorkingOrieCnstrObs->getListError(); }

	//@}


protected:

	/*!@name Access methods */
	//@{
		/*! Returns a pointer to the working horizontal angle observations */
		TWorkingObservation<THorAngleObservation>*			getWorkingHorAngObs() const;
		/*! Returns a pointer to the working zenithal angle observations */
		TWorkingObservation<TZenithDistObservation>*				getWorkingZenDistObs() const;
		/*! Returns a pointer to the working spatial distance observations */
		TWorkingObservation<TSpatialDistObservation>*			getWorkingSpaDistObs() const;
		/*! Returns a pointer to the working horizontal distance observations */
		TWorkingObservation<THorizontalDistObservation>*				getWorkingHorDistObs() const;
		/*! Returns a pointer to the working vertical distance observations */
		TWorkingObservation<TVertDistObservation>*			getWorkingVertDistObs(bool isDLEV) const;
		/*! Returns a pointer to the working offset to vertical line observations */
		TWorkingObservation<TOffsetToVerLineObservation>*		getWorkingOffsetToVerLineObs() const;
		/*! Returns a pointer to the working offset to spatial line observations */
		TWorkingObservation<TOffsetToSpaLineObservation>*		getWorkingOffsetToSpaLineObs() const;
		/*! Returns a pointer to the working offset to vertical plane observations */
		TWorkingObservation<TOffsetToVerPlaneObservation>*	getWorkingOffsetToVerPlaneObs() const;
		/*! Returns a pointer to the working offset to theodolite plane observations */
		TWorkingObservation<TOffsetToTheoPlaneObservation>*	getWorkingOffsetToTheoPlaneObs() const;
		/*! Returns a pointer to the working gyro. orientation observations */
		TWorkingObservation<TGyroOrientationObservation>*		getWorkingGyroOrieObs() const;
		/*! Returns a pointer to the working radial offset constraint observations */
		TWorkingObservation<TRadialOffsetCnstrObservation>*	getWorkingRadOffCnstrObs() const;
		/*! Returns a pointer to the working radial offset constraint observations */
		TWorkingObservation<TOrientationCnstrObservation>*	getWorkingOrieCnstrObs() const;


	//@}

private:	

	TWorkingObservation<THorAngleObservation>* fWorkingHorAngObs; /*!< pointer to horizontal angle observations list */
	TWorkingObservation<TZenithDistObservation>* fWorkingZenDistObs; /*!< pointer to zenithal angle observations list */
	TWorkingObservation<TSpatialDistObservation>* fWorkingSpaDistObs; /*!< pointer to spatial distance observations list */
	TWorkingObservation<THorizontalDistObservation>* fWorkingHorDistObs; /*!< pointer to horizontal distance observations list */
	TWorkingObservation<TVertDistObservation>* fWorkingVertDistObs; /*!< pointer to vertical distance observations list */
	TWorkingObservation<TVertDistObservation>* fWorkingDLEVDistObs; /*!< pointer to vertical distance observations list */
	TWorkingObservation<TOffsetToVerLineObservation>* fWorkingOffsetToVerLineObs; /*!< pointer to offset to vertical line observations list */
	TWorkingObservation<TOffsetToSpaLineObservation>* fWorkingOffsetToSpaLineObs; /*!< pointer to offset to spatial line observations list */
	TWorkingObservation<TOffsetToVerPlaneObservation>* fWorkingOffsetToVerPlaneObs; /*!< pointer to offset to vertical plane observations list */
	TWorkingObservation<TOffsetToTheoPlaneObservation>* fWorkingOffsetToTheoPlaneObs; /*!< pointer to offset to theodolite plane observations list */
	TWorkingObservation<TGyroOrientationObservation>* fWorkingGyroOrieObs; /*!< pointer to orientation observations list */
	TWorkingObservation<TRadialOffsetCnstrObservation>* fWorkingRadOffCnstrObs; /*!< pointer to radial contraints observations list*/
	TWorkingObservation<TOrientationCnstrObservation>* fWorkingOrieCnstrObs; /*!< pointer to orientation contraints observations list*/


};

#endif	// SU_OBS_SET
