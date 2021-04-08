#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

enum class EGridValue : uint8
{
	Empty,
	Bomb,
	RevealedEmpty,
	RevealedBomb
};

enum class EGameResult : uint8
{
	Playing,
	Win,
	Lose
};

/**
 * Top level widget that holds the Minesweeper game.
 */
class MINESLATER_API SMineSlater : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SMineSlater) {}

	SLATE_END_ARGS()

	SMineSlater();
	virtual ~SMineSlater();

	void Construct(const FArguments& InArgs);

private:

	static TArray<int32> GetBombIndices(int32 NumBombs, int32 GridCount);
	void PopulateGridPanel();

	int32 HandleWidthSliderValue() const;
	int32 HandleHeightSliderValue() const;
	int32 HandleBombCountSliderValue() const;
	
	void HandleWidthSliderChanged(int32 NewValue);
	void HandleHeightSliderChanged(int32 NewValue);
	void HandleBombCountSliderChanged(int32 NewValue);

	ECheckBoxState HandleIsCheatingChecked() const;
	void HandleCheatStateChanged(ECheckBoxState NewState);

	void RecalculatePendingBombCount();

	bool GameSettingChangesPending() const;

	FReply HandleNewGameButtonClicked();

	void SetGridValue(int32 Column, int32 Row, EGridValue NewValue);
	EGridValue GetGridValue(int32 Column, int32 Row) const;
	int32 GetNumberOfAdjacentBombs(int32 Column, int32 Row) const;

	bool HandleButtonEnabled(int32 Column, int32 Row) const;
	FReply HandleButtonClicked(int32 Column, int32 Row);
	FText HandleCellText(int32 Column, int32 Row) const;
	FSlateColor HandleCellForegroundColor(int32 Column, int32 Row) const;

	EVisibility HandleGameResultOverlayVisibility() const;
	FText HandleGameResultText() const;
	FSlateColor HandleGameResultButtonBackgroundColor() const;

	void RevealCell(int32 Column, int32 Row);

private:

	int32 Width;
	int32 Height;
	int32 BombCount;
	int32 PendingWidth;
	int32 PendingHeight;
	int32 PendingBombCount;
	int32 NumReveals;
	TArray<EGridValue> GridValues;
	EGameResult GameResult;
	bool bCheat;

	TSharedPtr<class SUniformGridPanel> GridPanel;
};




