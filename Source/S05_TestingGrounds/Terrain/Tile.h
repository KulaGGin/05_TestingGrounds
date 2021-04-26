// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

class UActorPool;
UCLASS()
class S05_TESTINGGROUNDS_API ATile : public AActor
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	ATile();
public:	
    UFUNCTION(BlueprintCallable, Category = "Weapon")
	void PlaceActors(TSubclassOf<AActor> ToSpawn, int MinSpawn, int MaxSpawn, float Radius, float MinScale, float MaxScale);

protected:
	// Called when the game starts or when spaconst TSubclassOf<AActor>&wned
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	UPROPERTY(EditDefaultsOnly, Category = "Spawming");
	FVector MinExtent = FVector(0, -2000, 0);
	UPROPERTY(EditDefaultsOnly, Category = "Spawming");
	FVector MaxExtent = FVector(4000, 2000, 0);
private:
	bool CanSpawnAtLocation(FVector GlobalLocation, float Radius);

    bool FindEmptyLocation(FVector& OutLocation, float Radius);
	void PlaceActor(TSubclassOf<AActor> ToSpawn, FVector SpawnPoint, float Rotation, float Scale);
    void PositionNavMeshBoundsVolume();

    UFUNCTION(BlueprintCallable, Category = "Pool")
	void SetPool(UActorPool* InPool);

	UActorPool* Pool;
	AActor* NavMeshBoundsVolume;
};
