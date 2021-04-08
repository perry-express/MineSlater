#include "SMineSlater.h"

#include "MineSlaterStyle.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"

#define LOCTEXT_NAMESPACE "MineSlater"

static constexpr int32 GDefaultWidth = 4;
static constexpr int32 GDefaultHeight = 4;
static constexpr int32 GDefaultBombCount = 4;

SMineSlater::SMineSlater()
	: Width(GDefaultWidth)
	, Height(GDefaultHeight)
	, BombCount(GDefaultBombCount)
	, PendingWidth(GDefaultWidth)
	, PendingHeight(GDefaultHeight)
	, PendingBombCount(GDefaultBombCount)
	, NumReveals(0)
	, GameResult(EGameResult::Playing)
	, bCheat(false)
	, GridPanel(nullptr)
{
}

SMineSlater::~SMineSlater()
{
}

void SMineSlater::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
			.Padding(2.0f)
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(4.0f, 0.0f, 4.0f, 0.0f)
				[
					SNew(STextBlock)
					.Text(NSLOCTEXT("MineSlater", "Width", "Width: "))
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.MinDesiredWidth(50)
					.MaxDesiredWidth(100)
					[
						SNew(SSpinBox<int32>)
						.Value(this, &SMineSlater::HandleWidthSliderValue)
						.MinValue(2)
						.MaxValue(64)
						.Delta(1)
						.OnValueChanged(this, &SMineSlater::HandleWidthSliderChanged)
					]
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(4.0f, 0.0f, 4.0f, 0.0f)
				[
					SNew(STextBlock)
					.Text(NSLOCTEXT("MineSlater", "Height", "Height: "))
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.MinDesiredWidth(50)
					.MaxDesiredWidth(100)
					[
						SNew(SSpinBox<int32>)
						.Value(this, &SMineSlater::HandleHeightSliderValue)
						.MinValue(2)
						.MaxValue(64)
						.Delta(1)
						.OnValueChanged(this, &SMineSlater::HandleHeightSliderChanged)
					]
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(4.0f, 0.0f, 4.0f, 0.0f)
				[
					SNew(STextBlock)
					.Text(NSLOCTEXT("MineSlater", "BombCount", "Bomb Count: "))
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(4.0f, 0.0f, 4.0f, 0.0f)
				[
					SNew(SBox)
					.MinDesiredWidth(50)
					.MaxDesiredWidth(100)
					[
						SNew(SSpinBox<int32>)
						.Value(this, &SMineSlater::HandleBombCountSliderValue)
						.MinValue(1)
						.MaxValue(64)
						.Delta(1)
						.OnValueChanged(this, &SMineSlater::HandleBombCountSliderChanged)
					]
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(4.0f, 0.0f, 4.0f, 0.0f)
				[

					SNew(STextBlock)
					.Text(NSLOCTEXT("MineSlater", "Cheat", "Cheat: "))
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(4.0f, 0.0f, 4.0f, 0.0f)
				[
					SNew(SCheckBox)
					.IsChecked(this, &SMineSlater::HandleIsCheatingChecked)
					.OnCheckStateChanged(this, &SMineSlater::HandleCheatStateChanged)
				]
				

				+ SHorizontalBox::Slot()
				.Padding(4.0f, 0.0f, 4.0f, 0.0f)
				.HAlign(HAlign_Right)
				[
					SNew(SButton)
					.OnClicked(this, &SMineSlater::HandleNewGameButtonClicked)
					.TextStyle(FEditorStyle::Get(), "FlatButton.DefaultTextStyle")
					.ButtonStyle(FEditorStyle::Get(), "FlatButton.Success")
					.HAlign(HAlign_Center)
					.ToolTip(SNew(SToolTip)
						.Text_Lambda([&]
						{
							if(GameSettingChangesPending())
							{
								return NSLOCTEXT("MineSlater", "NewGameNewSettingsTooltip", "Start a new game with your modified settings.");
							}
							else
							{
								return NSLOCTEXT("MineSlater", "NewGameTooltip", "Start a new game.");
							}
						})
					)
					.Text(NSLOCTEXT("MineSlater", "NewGame", "New Game"))
				]
			]
		]
	+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			SNew(SOverlay)

			+ SOverlay::Slot()
			[			
				SAssignNew(GridPanel, SUniformGridPanel)
			]

			+ SOverlay::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(SBorder)
				.Visibility(this, &SMineSlater::HandleGameResultOverlayVisibility)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SButton)
						.OnClicked(this, &SMineSlater::HandleNewGameButtonClicked)
						.Text(this, &SMineSlater::HandleGameResultText)
						.TextStyle(FMineSlaterStyle::Get(), "MineSlater.GameResultText")
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						.ButtonColorAndOpacity(this, &SMineSlater::HandleGameResultButtonBackgroundColor)
						.ContentPadding(FMargin(10.0f, 10.0f, 10.0f, 10.0f))
						.DesiredSizeScale(FVector2D(4.0f, 4.0f))
					]
				]
			]
		]
	];

	PopulateGridPanel();
}

TArray<int32> SMineSlater::GetBombIndices(int32 NumBombs, int32 GridCount)
{
	TArray<int32> BombIndices;
	BombIndices.Reserve(NumBombs);

	TArray<int32> Indices;
	Indices.Reserve(GridCount);

	// would use std::iota but I don't like to include the standard library in UE4
	for(int32 Idx {0}; Idx < GridCount; Idx++)
	{
		Indices.Add(Idx);
	}

	while(BombIndices.Num() < NumBombs)
	{
		const auto Idx{ FMath::RandRange(0, Indices.Num()-1) };
		BombIndices.Add(Indices[Idx]);
		Indices.RemoveAt(Idx);
	}

	return BombIndices;
}

void SMineSlater::PopulateGridPanel()
{
	GameResult = EGameResult::Playing;
	NumReveals = 0;
	
	GridPanel->ClearChildren();

	const int32 NumValues = Width * Height;
	GridValues.SetNumUninitialized(NumValues, true);

	const auto BombIndices = GetBombIndices(BombCount, NumValues);
	auto GetCurrentGridValue = [&](int32 Column, int32 Row)
	{
		const auto Idx{ Column * Height + Row };

		if (BombIndices.Contains(Idx))
		{
			return EGridValue::Bomb;
		}

		return EGridValue::Empty;
	};

	for (int32 Column{ 0 }; Column < Width; Column++)
	{
		for(int32 Row{0}; Row < Height; Row++)
		{
			const EGridValue CurrentValue = GetCurrentGridValue(Column, Row);
			SetGridValue(Column, Row, CurrentValue);
			
			GridPanel->AddSlot(Column, Row)
			[
				SNew(SButton)
				.IsEnabled_Lambda([this, Column, Row]
				{
					return HandleButtonEnabled(Column, Row);
				})
				.OnClicked_Lambda([this, Column, Row]
				{
					return HandleButtonClicked(Column, Row);
				})
				.ButtonStyle(FMineSlaterStyle::Get(), "MineSlater.CellButtonStyle")
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Visibility(EVisibility::HitTestInvisible)
					.Text_Lambda([this, Column, Row]
					{
						return HandleCellText(Column, Row);
					})
					.ColorAndOpacity_Lambda([this, Column, Row]
					{
						return HandleCellForegroundColor(Column, Row);
					})
					.TextStyle(FMineSlaterStyle::Get(), "MineSlater.DefaultCellTextStyle")
				]
			];
		}
	}
}

int32 SMineSlater::HandleWidthSliderValue() const
{
	return PendingWidth;
}

int32 SMineSlater::HandleHeightSliderValue() const
{
	return PendingHeight;
}

int32 SMineSlater::HandleBombCountSliderValue() const
{
	return PendingBombCount;
}

void SMineSlater::HandleWidthSliderChanged(int32 NewValue)
{
	PendingWidth = NewValue;
	RecalculatePendingBombCount();
}

void SMineSlater::HandleHeightSliderChanged(int32 NewValue)
{
	PendingHeight = NewValue;
	RecalculatePendingBombCount();
}

void SMineSlater::HandleBombCountSliderChanged(int32 NewValue)
{
	PendingBombCount = NewValue;
}

ECheckBoxState SMineSlater::HandleIsCheatingChecked() const
{
	return bCheat ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void SMineSlater::HandleCheatStateChanged(ECheckBoxState NewState)
{
	bCheat = NewState == ECheckBoxState::Checked;
}

void SMineSlater::RecalculatePendingBombCount()
{
	const int32 PendingSize = PendingWidth * PendingHeight;
	PendingBombCount = FMath::Clamp(FMath::RoundToInt(0.25f * FMath::Sqrt(PendingSize)), 1, PendingSize-1);
}

bool SMineSlater::GameSettingChangesPending() const
{
	return Width != PendingWidth
		|| Height != PendingHeight
		|| BombCount != PendingBombCount;
}

FReply SMineSlater::HandleNewGameButtonClicked()
{
	Width = PendingWidth;
	Height = PendingHeight;
	BombCount = PendingBombCount;

	PopulateGridPanel();
	
	return FReply::Handled();
}

void SMineSlater::SetGridValue(int32 Column, int32 Row, EGridValue NewValue)
{
	const auto Idx{ Column * Height + Row };
	check(GridValues.IsValidIndex(Idx));
	GridValues[Idx] = NewValue;
}

EGridValue SMineSlater::GetGridValue(int32 Column, int32 Row) const
{
	const auto Idx{ Column * Height + Row };
	check(GridValues.IsValidIndex(Idx));
	return GridValues[Idx];
}

int32 SMineSlater::GetNumberOfAdjacentBombs(int32 Column, int32 Row) const
{
	int32 Sum{ 0 };

	auto IncrementSumIfBomb = [&Sum, this](int32 Column, int32 Row)
	{
		if(Column < 0 || Column >= Width || Row < 0 || Row >= Height)
		{
			return;
		}
		
		const auto Idx{ Column * Height + Row };
		
		if (GridValues.IsValidIndex(Idx) && (GridValues[Idx] == EGridValue::Bomb || GridValues[Idx] == EGridValue::RevealedBomb))
		{
			++Sum;
		}
	};

	static const TArray<int32> ColumnsOffsets { -1, 0, 1 };

	for(auto It = ColumnsOffsets.CreateConstIterator(); It; ++It)
	{
		IncrementSumIfBomb(Column + *It, Row - 1);
	}

	IncrementSumIfBomb(Column - 1, Row);
	IncrementSumIfBomb(Column + 1, Row);

	for (auto It = ColumnsOffsets.CreateConstIterator(); It; ++It)
	{
		IncrementSumIfBomb(Column + *It, Row + 1);
	}

	return Sum;
}

bool SMineSlater::HandleButtonEnabled(int32 Column, int32 Row) const
{
	const auto GridValue = GetGridValue(Column, Row);
	return GridValue == EGridValue::Empty || GridValue == EGridValue::Bomb;
}

FReply SMineSlater::HandleButtonClicked(int32 Column, int32 Row)
{
	const auto GridValue = GetGridValue(Column, Row);
	
	if(GridValue == EGridValue::Empty)
	{
		RevealCell(Column, Row);
		
		if(NumReveals == (Width*Height) - BombCount)
		{
			GameResult = EGameResult::Win;
		}
	}
	else if(GridValue == EGridValue::Bomb)
	{
		SetGridValue(Column, Row, EGridValue::RevealedBomb);
		GameResult = EGameResult::Lose;
	}

	return FReply::Handled();
}

FText SMineSlater::HandleCellText(int32 Column, int32 Row) const
{
	const auto GridValue = GetGridValue(Column, Row);
	if (bCheat)
	{
		switch (GridValue)
		{
		case EGridValue::Empty:
		case EGridValue::RevealedEmpty:
			return FText::FromString(FString::Printf(TEXT("%d <%d,%d>"), GetNumberOfAdjacentBombs(Column, Row), Column, Row));
		case EGridValue::Bomb:
			return FText::FromString(FString::Printf(TEXT("B <%d,%d>"), Column, Row));
		case EGridValue::RevealedBomb:
			return FText::FromString(FString::Printf(TEXT("X <%d,%d>"), Column, Row));
		default:
			checkNoEntry();
			return FText::GetEmpty();
		}
	}
	else
	{
		switch (GridValue)
		{
		case EGridValue::Empty:
		case EGridValue::Bomb:
			return FText::GetEmpty();
		case EGridValue::RevealedBomb:
			return FText::FromString("X");
		case EGridValue::RevealedEmpty:
			return FText::AsNumber(GetNumberOfAdjacentBombs(Column, Row));
		default:
			checkNoEntry();
			return FText::GetEmpty();
		}
	}
}

FSlateColor SMineSlater::HandleCellForegroundColor(int32 Column, int32 Row) const
{
	const auto GridValue = GetGridValue(Column, Row);
	
	if(GridValue == EGridValue::Bomb)
	{
		return FMineSlaterStyle::Get().GetSlateColor("MineSlater.BombTextColor");
	}
	
	if (GridValue == EGridValue::RevealedEmpty)
	{
		const auto NumAdjacentBombs = GetNumberOfAdjacentBombs(Column, Row);
		if(NumAdjacentBombs == 1)
		{
			return FMineSlaterStyle::Get().GetSlateColor("MineSlater.Adjacent1TextColor");
		}
		if (NumAdjacentBombs == 2)
		{
			return FMineSlaterStyle::Get().GetSlateColor("MineSlater.Adjacent2TextColor");
		}
		if (NumAdjacentBombs > 2)
		{
			return FMineSlaterStyle::Get().GetSlateColor("MineSlater.Adjacent3TextColor");
		}
	}

	return FSlateColor();
}

EVisibility SMineSlater::HandleGameResultOverlayVisibility() const
{
	return GameResult == EGameResult::Playing ? EVisibility::Hidden : EVisibility::Visible;
}

FText SMineSlater::HandleGameResultText() const
{
	if(GameResult == EGameResult::Win)
	{
		return NSLOCTEXT("MineSlater", "GameResultWin", "You Win!");
	}
	if (GameResult == EGameResult::Lose)
	{
		return NSLOCTEXT("MineSlater", "GameResultWin", "You Lose!");
	}

	return FText::GetEmpty();
}

FSlateColor SMineSlater::HandleGameResultButtonBackgroundColor() const
{
	if (GameResult == EGameResult::Win)
	{
		return FSlateColor(FLinearColor::Green);
	}
	if (GameResult == EGameResult::Lose)
	{
		return FSlateColor(FLinearColor::Red);
	}

	return FSlateColor();
}

void SMineSlater::RevealCell(int32 Column, int32 Row)
{
	SetGridValue(Column, Row, EGridValue::RevealedEmpty);
	++NumReveals;

	auto MaybeReveal = [this](int32 Column, int32 Row)
	{
		const auto Idx{ Column * Height + Row };
		
		if(!GridValues.IsValidIndex(Idx))
		{
			return;
		}
		
		const auto GridValue = GridValues[Idx];

		if (GridValue == EGridValue::Empty)
		{
			SetGridValue(Column, Row, EGridValue::RevealedEmpty);
			++NumReveals;
		}
	};

	MaybeReveal(Column - 1, Row);
	MaybeReveal(Column + 1, Row);
	MaybeReveal(Column, Row - 1);
	MaybeReveal(Column, Row + 1);
}


#undef LOCTEXT_NAMESPACE
