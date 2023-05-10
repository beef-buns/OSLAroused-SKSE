#pragma once
#include "PCH.h"
#include "Utilities/LRUCache.h"

bool IsActorNaked(RE::Actor* actorRef);

const static RE::FormID kActorTypeCreatureKeywordFormId = 0x13795;
const static RE::FormID kActorTypeAnimalKeywordFormId = 0x13798;
const static RE::FormID DominantFactionFormId = 0x8F88A;
const static RE::FormID SubmissiveFactionFormId = 0x8F88B;
const static RE::FormID AttractedToMalesFactionFormId = 0x8F8A2;
const static RE::FormID AttractedToFemalesFactionFormId = 0x8F8A3;

class ActorStateManager
{
public:
	static ActorStateManager* GetSingleton()
	{
		static ActorStateManager singleton;
		return &singleton;
	}

	ActorStateManager() :
		m_ActorNakedStateCache(std::function<bool(RE::Actor*)>(IsActorNaked), 100),
		m_CreatureKeyword((RE::BGSKeyword*)RE::TESForm::LookupByID(kActorTypeCreatureKeywordFormId)),
		m_AnimalKeyword((RE::BGSKeyword*)RE::TESForm::LookupByID(kActorTypeAnimalKeywordFormId)),
		m_DominantFaction((RE::TESFaction*)RE::TESForm::LookupByID(DominantFactionFormId)),
		m_SubmissiveFaction((RE::TESFaction*)RE::TESForm::LookupByID(SubmissiveFactionFormId)),
		m_AttractedToMalesFaction((RE::TESFaction*)RE::TESForm::LookupByID(AttractedToMalesFactionFormId)),
		m_AttractedToFemalesFaction((RE::TESFaction*)RE::TESForm::LookupByID(AttractedToFemalesFactionFormId))
	{}

	bool GetActorNaked(RE::Actor* actorRef);
	void ActorNakedStateChanged(RE::Actor* actorRef, bool newNaked);

	bool GetActorSpectatingNaked(RE::Actor* actorRef);
	void UpdateActorsSpectating(std::set<RE::Actor*> spectators);

	//Returns true if actor is non-creature, non-animal npc
	bool IsHumanoidActor(RE::Actor* actorRef);
	bool IsAttractedToMen(RE::Actor* actorRef);
	bool IsAttractedToWomen(RE::Actor* actorRef);

private:
	Utilities::LRUCache<RE::Actor*, bool> m_ActorNakedStateCache;

	std::map<RE::Actor*, float> m_NakedSpectatingMap;

	RE::BGSKeyword* m_CreatureKeyword;
	RE::BGSKeyword* m_AnimalKeyword;

	RE::TESFaction* m_DominantFaction;
	RE::TESFaction* m_SubmissiveFaction;
	RE::TESFaction* m_AttractedToMalesFaction;
	RE::TESFaction* m_AttractedToFemalesFaction;
};
