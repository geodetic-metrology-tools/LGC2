// SPDX-FileCopyrightText: 2025 CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

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

} // namespace tut