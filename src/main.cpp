#include "Hooks.h"

#include "Papyrus.h"
#include "Papyrus/PapyrusActor.h"
#include "Papyrus/PapyrusConfig.h"
#include "Papyrus/PapyrusInterface.h"
#include "PersistedData.h"
#include "RuntimeEvents.h"
#include "Utilities/Utils.h"

using namespace SKSE::log;

void InitializeLog()
{
	auto path = logger::log_directory();
	if (!path) {
		util::report_and_fail("Failed to find standard logging directory"sv);
	}

	auto* plugin = SKSE::PluginDeclaration::GetSingleton();
	*path /= fmt::format("{}.log", plugin->GetName());

	// *path /= plugin->GetName();
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
	const auto level = spdlog::level::info;

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
	log->set_level(level);
	log->flush_on(level);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%T] [%^%l%$] %v"s);
}

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
	InitializeLog();
	auto* plugin = SKSE::PluginDeclaration::GetSingleton();
	//SKSE::log::info(plugin->GetName());
	if (skse->IsEditor()) {
		critical("Loaded in editor, marking as incompatible");
		return false;
	}

	info("{} v{}", plugin->GetName(), plugin->GetVersion());

	SKSE::Init(skse);

	const auto papyrus = SKSE::GetPapyrusInterface();
	papyrus->Register(Papyrus::RegisterFunctions);
	papyrus->Register(PapyrusInterface::RegisterFunctions);
	papyrus->Register(PapyrusConfig::RegisterFunctions);
	papyrus->Register(PapyrusActor::RegisterFunctions);

	const auto serialization = SKSE::GetSerializationInterface();
	serialization->SetUniqueID(PersistedData::kArousalDataKey);
	serialization->SetSaveCallback(PersistedData::SaveCallback);
	serialization->SetLoadCallback(PersistedData::LoadCallback);
	serialization->SetRevertCallback(PersistedData::RevertCallback);

	// Hooks::ActorEquipHook::InstallHook();

	SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message* message) {
		if (message->type == SKSE::MessagingInterface::kDataLoaded) {
			RuntimeEvents::OnEquipEvent::RegisterEvent();
			WorldChecks::ArousalUpdateTicker::GetSingleton()->Start();
			logger::info("kDataLoaded Sanity Check");
		}

		if (message->type == SKSE::MessagingInterface::kPostLoadGame) {
			Utilities::Keywords::DistributeKeywords();
			logger::info("kPostLoadGame Sanity Check");
		}
	});

	return true;
}
