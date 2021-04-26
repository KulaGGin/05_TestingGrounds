// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"
#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "../ActorPool.h"
#include "Chaos/AABB.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}


template<class T>
void ATile::RandomlyPlaceActors(TSubclassOf<T> ToSpawn, int MinSpawn, int MaxSpawn, float Radius, float MinScale, float MaxScale)
{
	int NumberToSpawn = FMath::RandRange(MinSpawn, MaxSpawn);
	for(int i = 0; i < NumberToSpawn; ++i) {
		FSpawnPosition SpawnPosition{};

		SpawnPosition.Scale = FMath::RandRange(MinScale, MaxScale);

		bool Found = FindEmptyLocation(SpawnPosition.Location, SpawnPosition.Scale * Radius);

		if(Found)
		{
			SpawnPosition.Rotation = FMath::RandRange(-180.f, 180.f);
			PlaceActor(ToSpawn, SpawnPosition);
		}
	}
}

void ATile::PlaceActors(TSubclassOf<AActor> ToSpawn, int MinSpawn = 1, int MaxSpawn = 1, float Radius = 500, float MinScale = 1, float MaxScale = 1) {
	RandomlyPlaceActors(ToSpawn, MinSpawn, MaxSpawn, Radius, MinScale, MaxScale);
}

void ATile::PlaceAIPawns(TSubclassOf<APawn> ToSpawn, int MinSpawn, int MaxSpawn, float Radius)
{
	RandomlyPlaceActors(ToSpawn, MinSpawn, MaxSpawn, Radius, 1, 1);
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

void ATile::PlaceActor(TSubclassOf<AActor> ToSpawn, const FSpawnPosition& SpawnPosition)
{
	AActor* Spawned = GetWorld()->SpawnActor<AActor>(ToSpawn);
	if(Spawned)
	{
		Spawned->SetActorRelativeLocation(SpawnPosition.Location);
		Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		Spawned->SetActorRotation(FRotator(0, SpawnPosition.Rotation, 0));
		Spawned->SetActorScale3D(FVector(SpawnPosition.Scale));
	}
}

void ATile::PlaceActor(TSubclassOf<APawn> ToSpawn, const FSpawnPosition& SpawnPosition)
{
	FRotator Rotation = FRotator(0, SpawnPosition.Rotation, 0);
	APawn* Spawned = GetWorld()->SpawnActor<APawn>(ToSpawn, SpawnPosition.Location, Rotation);
	if(Spawned)
	{
		Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		Spawned->SpawnDefaultController();
		Spawned->Tags.Push(FName("Enemy"));
	}
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

TArray<FSpawnPosition> ATile::RandomSpawnPositions(int MinSpawn, int MaxSpawn, float Radius, float MinScale, float MaxScale)
{
	TArray<FSpawnPosition> SpawnPositions;

	int NumberToSpawn = FMath::RandRange(MinSpawn, MaxSpawn);
	for(int i = 0; i < NumberToSpawn; ++i) {
		FSpawnPosition SpawnPosition{};

		SpawnPosition.Scale = FMath::RandRange(MinScale, MaxScale);

		bool Found = FindEmptyLocation(SpawnPosition.Location, SpawnPosition.Scale * Radius);

		if(Found)
		{
			SpawnPosition.Rotation = FMath::RandRange(-180.f, 180.f);
			SpawnPositions.Add(SpawnPosition);
		}
	}

	return SpawnPositions;
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

	if(NavMeshBoundsVolume && Pool)
	{
		Pool->Return(NavMeshBoundsVolume);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s]: tried to return null"), *GetName());
	}
}

