// Copyright Epic Games, Inc. All Rights Reserved.

#include "AIDemoGameMode.h"
#include "AIDemoCharacter.h"
#include "UObject/ConstructorHelpers.h"

#include "AICharacter.h"
#include "Kismet/GameplayStatics.h"

AAIDemoGameMode::AAIDemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AAIDemoGameMode::AddAIDeath()
{
	NumAIDead++;

	if (NumAIDead == NumAI)
	{
		/*ResetLevel();*/
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
	}
}

void AAIDemoGameMode::BeginPlay()
{
	NumAIDead = 0;
}
