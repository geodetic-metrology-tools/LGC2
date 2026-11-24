// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include <cmath>

#include <TLGCCalculation.h>
#include <TLGCData.h>
#include <TReader.h>
#include <testStatistics.h>

namespace tut
{
struct test_Stat
{
};
typedef test_group<test_Stat> factory;
typedef factory::object object;
} // namespace tut

namespace
{
tut::factory tf("Statistics tests");
}

namespace tut
{
template<>
template<>
void object::test<1>()
{
	std::shared_ptr<TLGCData> projTest(new TLGCData);

	set_test_name("statistics test with default values");
	TReader r(projTest);

	std::stringstream infiler(Statistics::alpha_beta_default);
	r.read(infiler);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	const TLGCData &dataset = calcul.getData();
	double hasStat = dataset.getConfig().faut.isActive();
	ensure("Statistics should be active.", hasStat);

	// look at the statistics
	double alpha = dataset.getConfig().faut.alpha;
	double beta = dataset.getConfig().faut.beta;
	const TLGCStatistic &stat = dataset.getStatistics();
	bool obsXIsFlagged = stat.getGToCompute(0);
	bool obsYIsFlagged = stat.getGToCompute(1);
	double wy = stat.getWi()(1);
	double nablaY = stat.getNabla()(1);
	double deltY = stat.getDelty()(1);
	ensure_equals("alpha should be at default value 0.01 = 1%", alpha, 0.01);
	ensure_equals("beta should be at default value 0.1 = 10%", beta, 0.1);
	ensure("x observation should not be flagged as blunder", !obsXIsFlagged);
	ensure("y observation should be flagged as blunder", obsYIsFlagged);
	ensure_equals("wi of y observation should be -2.58 ", wy, -2.58, 1e-3);
	ensure_equals("nabla of y observation should be approximately 3.857mm", M2MM * nablaY, 3.857, 1e-2);
	ensure_equals("deltY of y observation should be 0 (observation between two fixed points)", deltY, 0.0);
}

template<>
template<>
void object::test<2>()
{
	std::shared_ptr<TLGCData> projTest(new TLGCData);

	set_test_name("statistics test with non-standard values");
	TReader r(projTest);

	std::stringstream infiler(Statistics::alpha_beta_non_default);
	r.read(infiler);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	const TLGCData &dataset = calcul.getData();
	double hasStat = dataset.getConfig().faut.isActive();
	ensure("Statistics should be active.", hasStat);

	// look at the statistics
	double alpha = dataset.getConfig().faut.alpha;
	double beta = dataset.getConfig().faut.beta;
	const TLGCStatistic &stat = dataset.getStatistics();
	bool obsXIsFlagged = stat.getGToCompute(0);
	bool obsYIsFlagged = stat.getGToCompute(1);
	double wy = stat.getWi()(1);
	double nablaY = stat.getNabla()(1);
	double deltY = stat.getDelty()(1);
	ensure_equals("alpha should be at default value 0.02 = 2%", alpha, 0.02);
	ensure_equals("beta should be at default value 0.2 = 20%", beta, 0.2);
	ensure("x observation should not be flagged as blunder", !obsXIsFlagged);
	ensure("y observation should be flagged as blunder", obsYIsFlagged);
	ensure_equals("wi of y observation should be -2.326 ", wy, -2.326, 1e-3);
	ensure_equals("nabla of y observation should be approximately 3.168mm", M2MM * nablaY, 3.168, 1e-2);
	ensure_equals("deltY of y observation should be 0 (observation between two fixed points)", deltY, 0.0);
}

template<>
template<>
void object::test<3>()
{
	std::shared_ptr<TLGCData> projTest(new TLGCData);

	set_test_name("statistics of undetermined observations are NO_VAL");
	TReader r(projTest);

	// origin is only observed by its own OBSXYZ: no redundancy, so its 3 observations are undetermined
	std::stringstream infiler(Statistics::alpha_beta_default);
	r.read(infiler);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	const TLGCStatistic &stat = calcul.getData().getStatistics();
	ensure_equals("6 observations expected", static_cast<int>(stat.getWi().size()), 6);

	for (int i = 0; i < 3; i++)
		ensure("refPt observations should be determined", stat.getAreDetermined(i));
	// x observation of refPt is not flagged, so its G value is not computed
	ensure("G of a non flagged observation should be NO_VAL", std::isnan(stat.getGi()(0)));

	for (int i = 3; i < 6; i++)
	{
		ensure("origin observations should be undetermined", !stat.getAreDetermined(i));
		ensure("G should not be computed", !stat.getGToCompute(i));
		ensure("Delta should not be computed", !stat.getDeltaComputed(i));
		ensure("Z should be NO_VAL", std::isnan(stat.getZi()(i)));
		ensure("W should be NO_VAL", std::isnan(stat.getWi()(i)));
		ensure("T should be NO_VAL", std::isnan(stat.getTi()(i)));
		ensure("DELTY should be NO_VAL", std::isnan(stat.getDelty()(i)));
		ensure("NABLA should be NO_VAL", std::isnan(stat.getNabla()(i)));
		ensure("G should be NO_VAL", std::isnan(stat.getGi()(i)));
	}
}

template<>
template<>
void object::test<4>()
{
	std::shared_ptr<TLGCData> projTest(new TLGCData);

	set_test_name("global statistics are NO_VAL without *FAUT");
	TReader r(projTest);

	std::stringstream infiler(Statistics::no_faut);
	r.read(infiler);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	const TLGCData &dataset = calcul.getData();
	ensure("Statistics should not be active.", !dataset.getConfig().faut.isActive());

	const TLGCStatistic &stat = dataset.getStatistics();
	ensure("OVERALL should be NO_VAL", std::isnan(stat.getOVERALL()));
	ensure("DOF should be NO_VAL", std::isnan(stat.getDOF()));
}

} // namespace tut
