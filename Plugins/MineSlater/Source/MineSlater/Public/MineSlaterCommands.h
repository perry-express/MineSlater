// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "MineSlaterStyle.h"

class FMineSlaterCommands : public TCommands<FMineSlaterCommands>
{
public:

	FMineSlaterCommands()
		: TCommands<FMineSlaterCommands>(TEXT("MineSlater"), NSLOCTEXT("Contexts", "MineSlater", "MineSlater Plugin"), NAME_None, FMineSlaterStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};