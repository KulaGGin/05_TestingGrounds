// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"
#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "../ActorPool.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ATile::PlaceActors(TSubclassOf<AActor> ToSpawn, int MinSpawn = 1, int MaxSpawn = 1, float Radius = 500, float MinScale = 1, float MaxScale = 1) {

	int NumberToSpawn = FMath::RandRange(MinSpawn, MaxSpawn);

    for(int i = 0; i < NumberToSpawn; ++i) {
		FVector SpawnPoint;
		float RandomScale = FMath::RandRange(MinScale, MaxScale);
        bool Found = FindEmptyLocation(SpawnPoint, RandomScale * Radius);
		if(Found)
		{
			float RandomRotation = FMath::RandRange(-180.f, 180.f);
			PlaceActor(ToSpawn, SpawnPoint, RandomRotation, RandomScale);
		}
    }
}

bool ATile::FindEmptyLocation(FVector& OutLocation, float Radius)
{
    FBox Bounds(MinExtent, MaxExtent);

	const int MAX_ATTEMPTS = 100;

	for(size_t i = 0; i < MAX_ATTEMPTS; ++i)
	{
	    FVector CandidatePoint = FMath::RandPointInBox(Bounds);
		if(CanSpawnAtLocation(CandidatePoint, Radius))
		{
			OutLocation = CandidatePoint;
			return true;
		}
	}

	return false;
}

void ATile::PlaceActor(TSubclassOf<AActor> ToSpawn, FVector SpawnPoint, float Rotation, float Scale)
{
	AActor* Spawned = GetWorld()->SpawnActor<AActor>(ToSpawn);
	Spawned->SetActorRelativeLocation(SpawnPoint);
	Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	Spawned->SetActorRotation(FRotator(0, Rotation, 0));
	Spawned->SetActorScale3D(FVector(Scale));
}

void ATile::PositionNavMeshBoundsVolume()
{
    NavMeshBoundsVolume = Pool->Checkout();
	if(!NavMeshBoundsVolume)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Not enough actors in pool."), *GetName());
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[%s] checked out %s"), *GetName(), *NavMeshBoundsVolume->GetName());
    NavMeshBoundsVolume->SetActorLocation(GetActorLocation() + NavigationBoundsOffset);
	FNavigationSystem::Build(*GetWorld());
}

void ATile::SetPool(UActorPool* InPool)
{
	UE_LOG(LogTemp, Warning, TEXT("[%s] Setting Pool %s"), *this->GetName(), *InPool->GetName());
	Pool = InPool;

	PositionNavMeshBoundsVolume();
}

bool ATile::CanSpawnAtLocation(FVector Location, float Radius) {
	FHitResult HitResult;
    FVector GlobalLocation = ActorToWorld().TransformPosition(Location);
    bool HasHit = GetWorld()->SweepSingleByChannel(
                                     HitResult,
                                     GlobalLocation,
                                     GlobalLocation,
                                     FQuat::Identity,
                                     ECollisionChannel::ECC_GameTraceChannel2,
                                     FCollisionShape::MakeSphere(Radius)
                                    );
	return !HasHit;
}

void ATile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if(NavMeshBoundsVolume)
	{
		Pool->Return(NavMeshBoundsVolume);
		UE_LOG(LogTemp, Warning, TEXT("[%s]: returned %s"), *GetName(), *NavMeshBoundsVolume->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s]: tried to return null"), *GetName());
	}
}

