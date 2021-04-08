// Copyright Epic Games, Inc. All Rights Reserved.

#include "MineSlaterCommands.h"

#define LOCTEXT_NAMESPACE "FMineSlaterModule"

void FMineSlaterCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "MineSlater", "Bring up MineSlater window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
