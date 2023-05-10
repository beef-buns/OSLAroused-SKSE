#pragma once

#include "Managers/LibidoManager.h"

namespace ArousalManager
{
	float GetArousal(RE::Actor* actorRef, bool bUpdateState = true);
	float SetArousal(RE::Actor* actorRef, float value);
	float ModifyArousal(RE::Actor* actorRef, float value);
	void UpdateArousal(RE::Actor* actor);

	float CalculateArousal(RE::Actor* actorRef, float timePassed);
}
