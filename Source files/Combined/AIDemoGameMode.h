// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AIDemoGameMode.generated.h"

UCLASS(minimalapi)
class AAIDemoGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAIDemoGameMode();

	UPROPERTY(EditAnywhere, Category = "AI_Characters")
		int NumAI = 0;

	void AddAIDeath();
protected:
	virtual void BeginPlay() override;

	int NumAIDead = 0;
};



