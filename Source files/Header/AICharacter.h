// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AICharacter.generated.h"

UENUM(BlueprintType)
enum class EMonsterState : uint8
{
	Idle,

	Patrolling,

	Chasing,

	MAX
};

class UAISenseConfig_Sight;
class UAIPerceptionComponent;

UCLASS()
class AIDEMO_API AAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAICharacter();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "After Perception")
        class USphereComponent* PerceptionRange;

    UPROPERTY(VisibleDefaultsOnly, Category = "Perception")
        UAIPerceptionComponent* PerceptionComponent;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay();

public:
    // Called every frame
    virtual void Tick(float DeltaTime);

    //Perception
    UPROPERTY()
        UAISenseConfig_Sight* SightConfig;
private:

    UFUNCTION()
        void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

protected:

   /* UFUNCTION()
        void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);*/

    UFUNCTION()
        void HandleColliderEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UFUNCTION()
        virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


    UPROPERTY()
        EMonsterState CurrentState = EMonsterState::MAX;

    UPROPERTY()
        EMonsterState DefaultState;

    void SetMonsterState(EMonsterState NewState);

public:

    void ActivatePerception();

protected:

    bool bIsOverlapping = false;

    FVector Direction;

    UPROPERTY(EditInstanceOnly, Category = "AID_AIControls")
        bool bPatrol = false;

    // The current point the actor is either moving to or standing at
    AActor* CurrentPatrolPoint;

    int CurrentPatrolPointIndex = 0;

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AID_AIControls")
    TArray<AActor*> PatrolPoints;

    UFUNCTION()
    void MoveToNextPatrolPoint();

    bool hasSeen = false;

    class AAIDemoCharacter* Character;

    FVector ResetLocation;

    FTimerHandle TimerHandle;

    float Health = 100.f;
};
