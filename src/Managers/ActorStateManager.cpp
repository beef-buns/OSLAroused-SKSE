#include "ActorStateManager.h"
#include "Utilities/Utils.h"
#include "Papyrus.h"
#include "Managers/LibidoManager.h"

bool IsActorNaked(RE::Actor* actorRef)
{
	return Utilities::Actor::IsNaked(actorRef);
}

bool ActorStateManager::GetActorNaked(RE::Actor* actorRef)
{
	return m_ActorNakedStateCache(actorRef);
}

void AddNakedPerks(RE::Actor* actorRef) {}

void RemoveNakedPerks(RE::Actor* actor) {}

void ActorStateManager::ActorNakedStateChanged(RE::Actor* actorRef, bool newNaked)
{
	m_ActorNakedStateCache.UpdateItem(actorRef, newNaked);

	if (newNaked) {
		AddNakedPerks(actorRef);

	} else {
		RemoveNakedPerks(actorRef);
	}

	Papyrus::Events::SendActorNakedUpdatedEvent(actorRef, newNaked);
	
	//Actor Naked updated so remove libido cache entry to force refresh on next fetch
	LibidoManager::GetSingleton()->ActorLibidoModifiersUpdated(actorRef);
}

bool ActorStateManager::GetActorSpectatingNaked(RE::Actor* actorRef)
{
	if (const auto lastViewedGameTime = m_NakedSpectatingMap[actorRef]) {
		//@TODO: Calculate time based off global update cycle [not just 0.72 game hours]
		if (RE::Calendar::GetSingleton()->GetCurrentGameTime() - lastViewedGameTime < 0.1f) {
			return true;
		}
	}
	return false;
}

void ActorStateManager::UpdateActorsSpectating(std::set<RE::Actor*> spectators)
{
	//Remove any old spectators from map who are not in spectators set
	//Need to do this to purge libido modifier cache
	for (auto itr = m_NakedSpectatingMap.begin(); itr != m_NakedSpectatingMap.end();) {
		if (!spectators.contains((*itr).first)) {
			LibidoManager::GetSingleton()->ActorLibidoModifiersUpdated((*itr).first);
			itr = m_NakedSpectatingMap.erase(itr);
		} else {
			itr++;
		}
	}

	float currentTime = RE::Calendar::GetSingleton()->GetCurrentGameTime();
	for (const auto spectator : spectators) {
		m_NakedSpectatingMap[spectator] = currentTime;
		LibidoManager::GetSingleton()->ActorLibidoModifiersUpdated(spectator);
	}
}

bool ActorStateManager::IsHumanoidActor(RE::Actor* actorRef)
{
	if (!m_CreatureKeyword) {
		m_CreatureKeyword = (RE::BGSKeyword*)RE::TESForm::LookupByID(kActorTypeCreatureKeywordFormId);
	}
	if (!m_AnimalKeyword) {
		m_AnimalKeyword = (RE::BGSKeyword*)RE::TESForm::LookupByID(kActorTypeAnimalKeywordFormId);
	}

	if (m_CreatureKeyword && m_AnimalKeyword) {
		return !actorRef->HasKeyword(m_CreatureKeyword) && !actorRef->HasKeyword(m_AnimalKeyword);
	}

	return false;
}
