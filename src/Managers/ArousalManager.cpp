#include "ArousalManager.h"
#include "Papyrus.h"
#include "PersistedData.h"
// #include "Settings.h"
#include "Utilities/Utils.h"

using namespace PersistedData;

namespace ArousalManager
{
	float GetArousal(RE::Actor* actorRef, bool bUpdateState)
	{
		if (!actorRef || actorRef->IsChild()) {
			return -2.f;
		}

		RE::FormID actorFormId = actorRef->formID;

		const auto LastCheckTimeData = LastCheckTimeData::GetSingleton();
		auto lastCheckTime = LastCheckTimeData->GetData(actorFormId, 0.f);
		float curTime = RE::Calendar::GetSingleton()->GetCurrentGameTime();
		float gameHoursPassed = (curTime - lastCheckTime) * 24;

		float newArousal = CalculateArousal(actorRef, gameHoursPassed);

		//If set to update state, or we have never checked (last check time is 0), then update the lastchecktime
		if (bUpdateState || lastCheckTime == 0.f) {
			LastCheckTimeData->SetData(actorFormId, curTime);
			SetArousal(actorRef, newArousal);

			LibidoManager::GetSingleton()->UpdateActorLibido(actorRef, gameHoursPassed, newArousal);
		}
		logger::debug("Got Arousal for {} val: {}", actorRef->GetDisplayFullName(), newArousal);
		return newArousal;
	}

	void UpdateArousal(RE::Actor* actor)
	{
		const auto LastCheckTimeData = LastCheckTimeData::GetSingleton();
		auto lastCheckTime = LastCheckTimeData->GetData(actor->GetFormID(), 0.f);
		float curTime = RE::Calendar::GetSingleton()->GetCurrentGameTime();
		float gameHoursPassed = (curTime - lastCheckTime) * 24;

		float newArousal = CalculateArousal(actor, gameHoursPassed);

		if (lastCheckTime == 0.f || gameHoursPassed > 0.5f) {
			LastCheckTimeData->SetData(actor->GetFormID(), curTime);
			SetArousal(actor, newArousal);

			LibidoManager::GetSingleton()->UpdateActorLibido(actor, gameHoursPassed, newArousal);
		}
	}

	float SetArousal(RE::Actor* actorRef, float value)
	{
		value = std::clamp(value, 0.0f, 100.f);

		ArousalData::GetSingleton()->SetData(actorRef->formID, value);

		Papyrus::Events::SendActorArousalUpdatedEvent(actorRef, value);

		return value;
	}

	float ModifyArousal(RE::Actor* actorRef, float modValue)
	{
		modValue *= PersistedData::ArousalMultiplierData::GetSingleton()->GetData(actorRef->formID, 1.f);

		float currentArousal = GetArousal(actorRef, false);
		return SetArousal(actorRef, currentArousal + modValue);
	}

	float CalculateArousal(RE::Actor* actorRef, float gameHoursPassed)
	{
		float currentArousal = ArousalData::GetSingleton()->GetData(actorRef->formID, -2.f);

		//If never calculated, regen
		if (currentArousal < -1) {
			currentArousal = Utilities::GenerateRandomFloat(10.f, 50.f);
			logger::debug("Random Arousal: {} Val: {}", actorRef->GetDisplayFullName(), currentArousal);
			return currentArousal;
		}

		float currentArousalBaseline = LibidoManager::GetSingleton()->GetBaselineArousal(actorRef);

		float epsilon = Settings::GetSingleton()->GetArousalChangeRate();
		logger::trace("CalculateArousal: epsilon: {}", epsilon);

		float t = 1.f - std::pow(epsilon, gameHoursPassed);
		float newArousal = std::lerp(currentArousal, currentArousalBaseline, t);
		logger::info("CalculateArousal: {} from: {} newArousal {} Diff: {}  t: {}", actorRef->GetDisplayFullName(), currentArousal, newArousal, newArousal - currentArousal, t);

		return newArousal;
	}
}
