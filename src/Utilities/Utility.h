#pragma once

enum class AREA_TYPE
{
	kAreaTypeChillyInterior = -1,
	kAreaTypeInterior = 0,
	kAreaTypeWarm = 1,
	kAreaTypeCool = 2,
	kAreaTypeFreezing = 3,
	kAreaTypeReach = 4
};

class Utility
{
public:
	RE::TESQuest* DA16;
	RE::TESQuest* MQ101;
	RE::TESQuest* RelationshipMarriageFIN;

	RE::BGSKeyword* LocTypeInn;
	RE::BGSKeyword* LocTypePlayerHouse;

	uintptr_t PlayerSingletonAddress;
	uintptr_t UISingletonAddress;
	uintptr_t MenuControlsSingletonAddress;
	uintptr_t DoCombatSpellApplyAddress;
	uintptr_t EnableFtAddress;
	uintptr_t IsFtEnabledAddress;

	bool WasInOblivion = false;
	bool DisableFastTravel = true;
	bool AutoStart = true;

	static Utility* GetSingleton()
	{
		static Utility playerStatus;
		return &playerStatus;
	}

	static RE::PlayerCharacter* GetPlayer()
	{
		REL::Relocation<RE::NiPointer<RE::PlayerCharacter>*> singleton{ Utility::GetSingleton()->PlayerSingletonAddress };
		return singleton->get();
	}

	static RE::UI* GetUI()
	{
		REL::Relocation<RE::NiPointer<RE::UI>*> singleton{ Utility::GetSingleton()->UISingletonAddress };
		return singleton->get();
	}

	static float GetRandomFloat(float min, float max)
	{
		return SKSE::stl::RNG::GetSingleton()->Generate<float>(min, max);
	}

	static void ShowNotification(RE::BGSMessage* msg, bool messageBox = false)
	{
		RE::BSString messageDesc;
		msg->GetDescription(messageDesc, msg);
		if (messageBox) {
			RE::DebugMessageBox(messageDesc.c_str());
			/*auto uiQueue = RE::UIMessageQueue::GetSingleton();
			uiQueue->AddMessage(RE::TutorialMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::k, uiQueue->CreateUIMessageData("TEST"));*/
		} else {
			RE::DebugNotification(messageDesc.c_str());
		}
	}

	static bool PlayerCanGetWellRested()
	{
		return !PlayerIsVampire() && !PlayerIsWerewolf();
	}

	static bool PlayerIsVampire()
	{
		auto util = Utility::GetSingleton();
		return util->IsVampireConditions->IsTrue(GetPlayer(), nullptr);
	}

	static bool PlayerIsLich()
	{
		auto util = Utility::GetSingleton();
		if (util->Undeath_LichPerk) {
			return GetPlayer()->HasPerk(util->Undeath_LichPerk);
		} else {
			return false;
		}
	}

	static bool PlayerIsInJail()
	{
		auto utility = Utility::GetSingleton();
		RE::ConditionCheckParams param(Utility::GetPlayer(), nullptr);
		bool inJail = utility->IsInJailCondition->IsTrue(param);
		return inJail;
	}

	static bool PlayerIsNearSpouse()
	{
		auto relMarriageQuest = Utility::GetSingleton()->RelationshipMarriageFIN;
		auto interestString = RE::BSFixedString("LoveInterest");

		auto aliases = relMarriageQuest->aliases;
		RE::BGSBaseAlias* loveInterestBase = nullptr;

		for (auto alias : aliases) {
			if (alias && alias->aliasName == interestString) {
				loveInterestBase = alias;
				break;
			}
		}

		if (!loveInterestBase) {
			return false;
		}

		const auto loveInterestRef = skyrim_cast<RE::BGSRefAlias*>(loveInterestBase);

		if (!loveInterestRef) {
			return false;
		}

		if (relMarriageQuest->IsRunning() && relMarriageQuest->currentStage >= 10) {
			auto playerLoc = Utility::GetPlayer()->GetCurrentLocation();
			auto interest = loveInterestRef->GetActorReference();
			auto interestLoc = interest == nullptr ? nullptr : interest->GetCurrentLocation();

			if ((playerLoc && interestLoc) && playerLoc == interestLoc) {
				return true;
			}
		}

		return false;
	}

	static bool PlayerIsInHouseOrInn()
	{
		auto loc = Utility::GetPlayer()->GetCurrentLocation();
		auto util = Utility::GetSingleton();

		if (loc && (loc->HasKeyword(util->LocTypeInn) || loc->HasKeyword(util->LocTypePlayerHouse))) {
			return true;
		}

		return false;
	}

	static bool IsPlayerInDialogue()
	{
		return Utility::GetSingleton()->GetUI()->IsMenuOpen(RE::DialogueMenu::MENU_NAME);
	}

	static float GetWarmthRating(RE::Actor* actor)
	{
		using func_t = decltype(&Utility::GetWarmthRating);
		REL::Relocation<func_t> func{ Utility::GetSingleton()->GetWarmthRatingAddress };
		return func(actor);
	}

	static void DoCombatSpellApply(RE::Actor* actor, RE::SpellItem* spell, RE::TESObjectREFR* target)
	{
		using func_t = decltype(&Utility::DoCombatSpellApply);
		REL::Relocation<func_t> func{ Utility::GetSingleton()->DoCombatSpellApplyAddress };
		return func(actor, spell, target);
	}

	static void EnableFastTravel(bool a_enable)
	{
		using func_t = decltype(&Utility::EnableFastTravel);
		REL::Relocation<func_t> func{ Utility::GetSingleton()->EnableFtAddress };
		return func(a_enable);
	}

	static bool IsFastTravelEnabled()
	{
		using func_t = decltype(&Utility::IsFastTravelEnabled);
		REL::Relocation<func_t> func{ Utility::GetSingleton()->IsFtEnabledAddress };
		return func();
	}
};
