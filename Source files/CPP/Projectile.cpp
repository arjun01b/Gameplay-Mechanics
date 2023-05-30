// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AICharacter.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	ProjectileCollider->SetSphereRadius(25.0f);
	RootComponent = ProjectileCollider;

	ProjectileCollider->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SphereMesh->SetStaticMesh(SphereMeshAsset.Object);
	SphereMesh->SetupAttachment(ProjectileCollider);
	SphereMesh->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}

void AProjectile::SetDirection(FVector Direction)
{
	ProjectileDirection = Direction;
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ProjectileDirection != FVector::ZeroVector)
	{
		SetActorLocation(GetActorLocation() + ProjectileDirection * ProjectileSpeed);
	}
}

