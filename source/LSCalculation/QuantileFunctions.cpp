#include "QuantileFunctions.h"
#include <boost/math/distributions/chi_squared.hpp>
#include <boost/math/distributions/normal.hpp>
#include <boost/math/distributions/students_t.hpp>
namespace bm = boost::math;

double deviates_chi_sq(double p, double df)
{
	return bm::quantile(bm::chi_squared(df), p);
}

double deviates_students_t_lower_tail(double p, double df)
{
    return bm::quantile(bm::students_t(df), p);
}

double deviates_students_t_upper_tail(double p, double df)
{
    return bm::quantile(bm::students_t(df), 1-p);
}

double deviates_normal_upper_tail(double p)
{
    return bm::quantile(bm::normal(), 1-p);
}