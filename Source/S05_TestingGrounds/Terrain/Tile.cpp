// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"
#include "DrawDebugHelpers.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	auto SphereLocation = GetActorLocation();
	auto SphereRadius = 300.f;
	bool HasHit = CastSphere(SphereLocation, SphereRadius);
	HasHit = CastSphere(SphereLocation + FVector(0,0, 1000), SphereRadius);
}

void ATile::PlaceActors(TSubclassOf<AActor> ToSpawn, int MinSpawn, int MaxSpawn) {
	FVector Min(0, -2000, 0);
	FVector Max(4000, 2000, 0);
	FBox Bounds(Min, Max);
	int NumberToSpawn = FMath::RandRange(MinSpawn, MaxSpawn);

    for(int i = 0; i < NumberToSpawn; ++i) {
		FVector SpawnPoint = FMath::RandPointInBox(Bounds);
		AActor* Spawned = GetWorld()->SpawnActor<AActor>(ToSpawn, SpawnPoint, FRotator{});
		Spawned->SetActorRelativeLocation(SpawnPoint);
		Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
    }
}

bool ATile::CastSphere(FVector Location, float Radius) {
	FHitResult HitResult;
    bool HasHit = GetWorld()->SweepSingleByChannel(
                                     HitResult,
                                     Location,
                                     Location,
                                     FQuat::Identity,
                                     ECollisionChannel::ECC_GameTraceChannel2,
                                     FCollisionShape::MakeSphere(Radius)
                                    );
	FColor Color = HasHit ? FColor::Red : FColor::Green;
	DrawDebugCapsule(GetWorld(), Location, 0, Radius, FQuat::Identity, Color, true, 100);
	return HasHit;
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

