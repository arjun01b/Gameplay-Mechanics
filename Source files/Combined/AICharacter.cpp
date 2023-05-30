// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacter.h"

#include "AIDemoCharacter.h"

#include "Components/SphereComponent.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "DrawDebugHelpers.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Components/CapsuleComponent.h"

#include "Projectile.h"

#include "AIDemoGameMode.h"

// Sets default values
AAICharacter::AAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    PerceptionRange = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    PerceptionRange->SetSphereRadius(100.f);

    PerceptionRange->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    PerceptionRange->SetCollisionProfileName("OverlapAll");
    PerceptionRange->SetGenerateOverlapEvents(true);
    PerceptionRange->SetupAttachment(RootComponent);
    PerceptionRange->SetSimulatePhysics(false);

    PerceptionRange->OnComponentEndOverlap.AddDynamic(this, &AAICharacter::HandleColliderEndOverlap);


    // Perception
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConf"));
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
    PerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AAICharacter::OnPerceptionUpdated);
    SightConfig->SightRadius = 800.f;
    SightConfig->LoseSightRadius = 2000.0f;
    SightConfig->PeripheralVisionAngleDegrees = 135.0f;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    SightConfig->SetMaxAge(45.0f);
    PerceptionComponent->ConfigureSense(*SightConfig);

    GetCharacterMovement()->MaxWalkSpeed = 200.f;

    GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AAICharacter::OnHit);

    Tags.Add("Monster");
}

void AAICharacter::MoveToNextPatrolPoint()
{
    if (CurrentPatrolPointIndex >= PatrolPoints.Num() - 1)
    {
        CurrentPatrolPointIndex = 0;
    }
    else
    {
        CurrentPatrolPointIndex++;
    }

    CurrentPatrolPoint = PatrolPoints[CurrentPatrolPointIndex];
}

// Called when the game starts or when spawned
void AAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
    CurrentPatrolPointIndex = 0;

    if (bPatrol)
    {
        SetMonsterState(EMonsterState::Patrolling);
        MoveToNextPatrolPoint();
    }
    else
    {
        SetMonsterState(EMonsterState::Idle);
    }

    DefaultState = CurrentState;

    ResetLocation = GetActorLocation();
}

// Called every frame
void AAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (CurrentPatrolPoint != nullptr && bPatrol == true)
    {
        FVector Delta = GetActorLocation() - CurrentPatrolPoint->GetActorLocation();
        float DistanceToGoal = Delta.Size();

        if (DistanceToGoal < 100.f)
            MoveToNextPatrolPoint();

        UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), CurrentPatrolPoint);
    }
    /*else if (bPatrol == false)
    {
        MoveToNextPatrolPoint();
    }*/

    if (hasSeen == true)
    {
        bPatrol = false;

        UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), Character);

        FVector CharLoc = Character->GetActorLocation();
        FVector Loc = GetActorLocation();
        float distToChar = FVector::Distance(Loc, CharLoc);

        if (distToChar > SightConfig->LoseSightRadius)
        {
            UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), ResetLocation);
            SetMonsterState(DefaultState);
            hasSeen = false;
        }
    }
}

void AAICharacter::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    for (auto&& Target : UpdatedActors)
    {
        FActorPerceptionBlueprintInfo Info;
        PerceptionComponent->GetActorsPerception(Target, Info);
        for (const auto& Stimulus : Info.LastSensedStimuli)
        {
            if (Stimulus.Type == UAISense::GetSenseID(UAISense_Sight::StaticClass()))
            {
                bool bSenseResult = Stimulus.WasSuccessfullySensed();
                if (bSenseResult)
                {
                    DrawDebugCone(
                        GetWorld(),
                        GetActorLocation(),
                        -GetActorForwardVector(),
                        SightConfig->LoseSightRadius,
                        FMath::DegreesToRadians(SightConfig->PeripheralVisionAngleDegrees),
                        10.0f,
                        100.0f,
                        FColor::Red,
                        false,
                        5.0f
                    );
                    UE_LOG(LogTemp, Warning, TEXT("the actor enters the SIGHT sense range"));
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("the actor leaves the SIGHT sense range"));
                }
                //ENDIF
            }

            // Checking for Player
            if (Target->IsA<AAIDemoCharacter>())
            {
                Character = Cast<AAIDemoCharacter>(Target);

                hasSeen = true;
            }
        }
    }
}

void AAICharacter::HandleColliderEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor->IsA<AAIDemoCharacter>()) 
    {
        hasSeen = false;
        bPatrol = true;
    }
}

void AAICharacter::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor->IsA<AProjectile>()) 
    {
        OtherActor->Destroy();
        Health -= 50.f;
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("HEALTH REMAINING: %f"), Health));
    }

    if (Health <= 0.f)
    {
        AAIDemoGameMode* GameMode = (AAIDemoGameMode*)GetWorld()->GetAuthGameMode();
        GameMode->AddAIDeath();
        Destroy();
    }
}

void AAICharacter::SetMonsterState(EMonsterState NewState)
{
    if (NewState == CurrentState)
        return;

    CurrentState = NewState;
}

void AAICharacter::ActivatePerception()
{
    PerceptionComponent->Activate();
    SightConfig->SightRadius = 400.f;
    PerceptionComponent->ConfigureSense(*SightConfig);
}

