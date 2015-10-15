// TLSCalcWorkingUnknown.h

#ifndef SU_LS_WORK_UNKNOWN
#define SU_LS_WORK_UNKNOWN

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "lsalgo/TLSCalcLengthParam.h"
#include "lsalgo/TLSCalcOrientationParam.h"
#include "TLSCalcFreeVectorParam.h"

#include <unordered_map>

template <typename CalcParam>
class TLSCalcWorkingUnknown {

public:

	/*!@name Methods borrowed from the lists properties, named after them */
	//@{
	/*! Returns an iterator to the first element in the list */
	typename list<CalcParam>::iterator			begin() { return paramsList.begin(); }

	/*! Returns a const iterator to the first element in the list */
	typename list<CalcParam>::const_iterator	begin() const { return paramsList.begin(); }

	/*! Returns an iterator pointing after the last element in the list */
	typename list<CalcParam>::iterator			end() { return paramsList.end(); }

	/*! Returns a const iterator pointing after the last element in the list */
	typename list<CalcParam>::const_iterator	end() const { return paramsList.end(); }

	/*! Adds a new CalcParam at the end of the list, if it is not done yet (tests first if the object is
	already in the list or not)
	\returns an iterator to the CalcParam wether it was already inserted or just being inserted */
	typename list<CalcParam>::iterator			push_back(const CalcParam& param)
	{
		auto iter = calcParamsMap.find(param.getName());
		typename list<CalcParam>::iterator result;
		if (iter == calcParamsMap.end())
		{
			paramsList.push_back(param);
			result = --(paramsList.end());
			calcParamsMap[param.getName()] = result;
		}
		else
		{
			result = iter->second;
		}

		return result;
	}
	
	typename list<CalcParam>::iterator			push_back_unique(const CalcParam& param)
	{
		paramsList.push_back(param);

		return --(paramsList.end());
	}

	/*! Erases the selected CalcParam and deletes its corresponding pointer from the list */
	bool										erase(CalcParam& param)
	{
		auto iter = calcParamsMap.find(param.getName());
		if (iter != calcParamsMap.end())
		{
			paramsList.erase(iter->second);
			calcParamsMap.erase(param.getName());
			
			return true;
		}
		
		return false;
	}

	/*! Returns the size of the list*/
	size_t size() const { return paramsList.size(); }
	//@}

protected:

    std::unordered_map<string, typename list<CalcParam>::iterator> calcParamsMap;

	list<CalcParam>	paramsList; /*!< list of calc parameters */

};

typedef TLSCalcWorkingUnknown<TLSCalcLengthParam> TLSCalcWorkingLength;
typedef TLSCalcWorkingUnknown<TLSCalcOrientationParam> TLSCalcWorkingOrientation;
typedef TLSCalcWorkingUnknown<TLSCalcFreeVectorParam> TLSCalcWorkingFreeVec;

#endif //SU_LS_WORK_UNKNOWN
