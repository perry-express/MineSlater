#include "MineSlater.h"
#include "MineSlaterStyle.h"
#include "MineSlaterCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "SMineSlater.h"
#include "ToolMenus.h"

static const FName MineSlaterTabName("MineSlater");

#define LOCTEXT_NAMESPACE "FMineSlaterModule"

void FMineSlaterModule::StartupModule()
{
	FMineSlaterStyle::Initialize();
	FMineSlaterStyle::ReloadTextures();

	FMineSlaterCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FMineSlaterCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FMineSlaterModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMineSlaterModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(MineSlaterTabName, FOnSpawnTab::CreateRaw(this, &FMineSlaterModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FMineSlaterTabTitle", "MineSlater"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FMineSlaterModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FMineSlaterStyle::Shutdown();

	FMineSlaterCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(MineSlaterTabName);
}

TSharedRef<SDockTab> FMineSlaterModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SMineSlater)
		];
}

void FMineSlaterModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(MineSlaterTabName);
}

void FMineSlaterModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FMineSlaterCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FMineSlaterCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMineSlaterModule, MineSlater)