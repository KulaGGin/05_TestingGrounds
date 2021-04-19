// Fill out your copyright notice in the Description page of Project Settings.

#include "S05_TestingGrounds/Character/Mannequin.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "S05_TestingGrounds/Weapons/Gun.h"

// Sets default values
AMannequin::AMannequin()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a CameraComponent	
	FirstPersonCameraComponentNew = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponentNew->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponentNew->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponentNew->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1PNew = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1PNew->SetOnlyOwnerSee(true);
	Mesh1PNew->SetupAttachment(FirstPersonCameraComponentNew);
	Mesh1PNew->bCastDynamicShadow = false;
	Mesh1PNew->CastShadow = false;
	Mesh1PNew->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1PNew->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));
}

// Called when the game starts or when spawned
void AMannequin::BeginPlay()
{
	Super::BeginPlay();

	if(GunBlueprint == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Gun blueprint missing."));
		return;
	}
	Gun = GetWorld()->SpawnActor<AGun>(GunBlueprint);
	Gun->AttachToComponent(Mesh1PNew, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	Gun->AnimInstance = Mesh1PNew->GetAnimInstance();

	if(InputComponent) {
		// Bind fire event
		InputComponent->BindAction("Fire", IE_Pressed, this, &AMannequin::PullTrigger);
	}
}

void AMannequin::PullTrigger() {
	Gun->OnFire();
}

// Called every frame
void AMannequin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMannequin::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

