#include "core/world_controller.hpp"



WorldController::WorldController()
{
	primaryTick.SetPhase(ETickPhase::eInputParsing);
}

WorldController::~WorldController()
{}
