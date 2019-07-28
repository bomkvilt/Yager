#include "interface/ilifecycle.hpp"
#include <iostream>


ILifecycle::~ILifecycle()
{
	if (stage != ELifeStage::eInDestroy)
	{	//TODO:: use log
		std::cerr << "incorrect in-destructor lifecycle stage: '" + StageToName(stage) + "'";
		std::terminate();
	}
}

ELifeStage ILifecycle::GetLifeStage() const
{
	return stage;
}

void ILifecycle::SetLifeStage(ELifeStage newStage)
{
	static const auto translations = MakeTranslatoinTable();

	auto from = (UInt8)stage.load();
	auto to   = (UInt8)newStage;
	if (!translations[from][to])
	{
		throw std::runtime_error("inallowed lifecycle stage translation from '" + StageToName(stage) + "' to '" + StageToName(newStage) + "'");
	}
	stage = newStage;
}

constexpr std::array<std::array<bool, ELifeStageCount>, ELifeStageCount> ILifecycle::MakeTranslatoinTable()
{
	auto table = std::array<std::array<bool, ELifeStageCount>, ELifeStageCount> { {0}};
	auto allow = [&](ELifeStage from, ELifeStage to)
	{
		table[UInt8(from)][UInt8(to)] = true;
	};
	allow(ELifeStage::eNotInitialised, ELifeStage::eInDestroy);
	allow(ELifeStage::eNotInitialised, ELifeStage::eConstructed);
	allow(ELifeStage::eConstructed, ELifeStage::eInDestroy);
	allow(ELifeStage::eConstructed, ELifeStage::eActive);
	allow(ELifeStage::eActive, ELifeStage::ePaused);
	allow(ELifeStage::eActive, ELifeStage::eStoped);
	allow(ELifeStage::ePaused, ELifeStage::eActive);
	allow(ELifeStage::ePaused, ELifeStage::eStoped);
	allow(ELifeStage::eStoped, ELifeStage::eInDestroy);
	return table;
}

std::string ILifecycle::StageToName(ELifeStage stage)
{
	return stage == ELifeStage::eNotInitialised ? "not_initialised"
		: stage == ELifeStage::eConstructed ? "constructed"
		: stage == ELifeStage::eActive ? "active"
		: stage == ELifeStage::ePaused ? "paused"
		: stage == ELifeStage::eStoped ? "stopped"
		: "'unknown_stage'";
}
