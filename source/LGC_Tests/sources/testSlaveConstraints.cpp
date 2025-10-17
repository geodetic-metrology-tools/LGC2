// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include <TLGCData.h>
#include <TLGCFrameConstraintGroup.h>
#include <TReader.h>

#include "TLGCCalculation.h"
#include "testSlaveConstraints.h"

namespace tut
{
struct test_SLAVE
{
	test_SLAVE() : projTest(std::make_shared<TLGCData>()), reader(projTest) {}
	std::shared_ptr<TLGCData> projTest;
	TReader reader;
};
typedef test_group<test_SLAVE> factory;
typedef factory::object object;
} // namespace tut

namespace
{
tut::factory tf("Test slave constraints");
}

namespace tut
{
template<>
template<>
void object::test<1>()
{
	std::stringstream infiler(Slave::SLAVE_1);

	bool succesReading = reader.read(infiler);
	ensure_equals("Reading file successful", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	// test if the slave constraints worked
	std::list<TLGCFrameConstraintGroup> frameConstraints = projTest.get()->getSlaveGroups();

	for (TLGCFrameConstraintGroup group : frameConstraints)
	{
		std::string firstFrame = group.slaves.front();
		std::string secondFrame = group.slaves.back();
		TAdjustableHelmertTransformation master = projTest.get()->locateNode(firstFrame).node->data.get()->frame;
		TAdjustableHelmertTransformation slave = projTest.get()->locateNode(secondFrame).node->data.get()->frame;

		// identical freedoms?
		ensure_equals("Master and Slave need to have the same free parameters", true, master.getRelativeUnknIndices() == slave.getRelativeUnknIndices());

		// identical estimated free parameters?
		for (int idx = 0; idx < 3; idx++)
		{
			if (!master.isTranslationFixed(idx))
			{
				ensure_equals("Master and Slave need to have the same values for the free parameters", master.getEstTranslation(idx), slave.getEstTranslation(idx), 1e-9);
			}
			if (!master.isRotationFixed(idx))
			{
				ensure_equals("Master and Slave need to have the same values for the free parameters", master.getEstRotation(idx), slave.getEstRotation(idx), 1e-9);
			}
		}
		if (!master.isScaleFixed())
		{
			ensure_equals("Master and Slave need to have the same values for the free parameters", master.getEstScale(), slave.getEstScale(), 1e-9);
		}
	}
}

template<>
template<>
void object::test<2>()
{
	std::stringstream infiler(Slave::SLAVE_2);

	bool succesReading = reader.read(infiler);
	ensure_equals("Reading file successful", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	// calculation has to work, consistency check needs to be passed
	ensure_equals("Calculation has to work because Slave constraint fixes the otherwise free frame", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	// test if the slave constraints worked
	std::list<TLGCFrameConstraintGroup> frameConstraints = projTest.get()->getSlaveGroups();

	for (TLGCFrameConstraintGroup group : frameConstraints)
	{
		std::string firstFrame = group.slaves.front();
		std::string secondFrame = group.slaves.back();
		TAdjustableHelmertTransformation master = projTest.get()->locateNode(firstFrame).node->data.get()->frame;
		TAdjustableHelmertTransformation slave = projTest.get()->locateNode(secondFrame).node->data.get()->frame;

		// identical freedoms?
		ensure_equals("Master and Slave need to have the same free parameters", true, master.getRelativeUnknIndices() == slave.getRelativeUnknIndices());

		// identical estimated free parameters?
		for (int idx = 0; idx < 3; idx++)
		{
			if (!master.isTranslationFixed(idx))
			{
				ensure_equals("Master and Slave need to have the same values for the free parameters", master.getEstTranslation(idx), slave.getEstTranslation(idx), 1e-9);
			}
			if (!master.isRotationFixed(idx))
			{
				ensure_equals("Master and Slave need to have the same values for the free parameters", master.getEstRotation(idx), slave.getEstRotation(idx), 1e-9);
			}
		}
		if (!master.isScaleFixed())
		{
			ensure_equals("Master and Slave need to have the same values for the free parameters", master.getEstScale(), slave.getEstScale(), 1e-9);
		}
	}
}

template<>
template<>
void object::test<3>()
{
	std::stringstream infiler(Slave::SLAVE_incompatible);

	bool succesReading = reader.read(infiler);
	ensure_equals("Reader should raise error caused by incompatible degrees of freedom", succesReading, false);
}

}; // namespace tut
