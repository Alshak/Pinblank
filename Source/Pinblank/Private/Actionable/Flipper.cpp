// Copyright Alain SHAKOUR 2016 contact@alainshakour.com This software is a computer program whose purpose is entertainment.This software is governed by the CeCILL-C license under French law and abiding by the rules of distribution of free software.  You can  use, modify and/ or redistribute the software under the terms of the CeCILL-C license as circulated by CEA, CNRS and INRIA at the following URL "http://www.cecill.info". As a counterpart to the access to the source code and  rights to copy, modify and redistribute granted by the license, users are provided only with a limited warranty  and the software's author,  the holder of the economic rights,  and the successive licensors  have only  limited liability. In this respect, the user's attention is drawn to the risks associated with loading,  using,  modifying and/or developing or reproducing the software by the user in light of its specific status of free software, that may mean  that it is complicated to manipulate,  and  that  also therefore means  that it is reserved for developers  and  experienced professionals having in-depth computer knowledge. Users are therefore encouraged to load and test the software's suitability as regards their requirements in conditions enabling the security of their systems and/or data to be ensured and,  more generally, to use and operate it in the same conditions as regards security. The fact that you are presently reading this means that you have had knowledge of the CeCILL-C license and that you accept its terms.

#include "Pinblank.h"
#include "Flipper.h"

AFlipper::AFlipper()
{
	PrimaryActorTick.bCanEverTick = true;
		
	// Flipper mesh
	flipperMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("flipperMesh"));
	RootComponent = flipperMesh;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> flipperVisualAsset(TEXT("/Game/Meshes/flipper.flipper"));
	if (flipperVisualAsset.Succeeded())
	{
		flipperMesh->SetStaticMesh(flipperVisualAsset.Object);
		static ConstructorHelpers::FObjectFinder<UMaterial> MaterialResource(TEXT("/Game/Materials/M_MasterEnemy.M_MasterEnemy"));
		if (MaterialResource.Succeeded())
		{
			flipperMesh->SetMaterial(0, MaterialResource.Object);
		}
	}
	flipperMesh->SetWorldScale3D(FVector(0.08f));
	flipperMesh->SetWorldRotation(FRotator(0, 0, bottomAngle));
	flipperMesh->SetEnableGravity(false);
	flipperMesh->SetSimulatePhysics(false);
	flipperMesh->SetNotifyRigidBodyCollision(true);
	this->OnActorHit.AddDynamic(this, &AFlipper::OnHitActor);

	// Collider to detect ball position
	UCapsuleComponent* capsuleCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	capsuleCollider->AttachTo(RootComponent);
	capsuleCollider->SetCapsuleSize(CAPSULE_RADIUS, CAPSULE_HALF_HEIGHT);
	capsuleCollider->SetRelativeScale3D(FVector(60));
	capsuleCollider->SetRelativeRotation(FRotator(0, 0, -bottomAngle));
	capsuleCollider->SetEnableGravity(false);
}

void AFlipper::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	// Register the current Z rotation to allow rotation on this axis in the editor
	currentYaw = flipperMesh->GetComponentRotation().Yaw;
	flipperDestination = FRotator(0, currentYaw, bottomAngle);
}


void AFlipper::BeginPlay()
{
	Super::BeginPlay();	
}

void AFlipper::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	// Rotate to the top position
	flipperMesh->SetWorldRotation(FMath::RInterpConstantTo(flipperMesh->GetComponentRotation(), flipperDestination, DeltaTime,rotationInterpSpeed));

	// Return to bottom position if player stop holding interact button
	if (!bIsInteracted)
	{
		flipperDestination = FRotator(0, currentYaw, bottomAngle);
	}
}

void AFlipper::Interact(ABall* ball)
{
	// Set the top position as current destination
	bIsInteracted = true;
	flipperDestination = FRotator(0, currentYaw, topAngle);
}

void AFlipper::StopInteract(ABall* ball)
{
	bIsInteracted = false;
}

UStaticMeshComponent* AFlipper::GetColoredMesh()
{
	return flipperMesh;
}

void AFlipper::OnHitActor(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	// Add some impulse on ball hit
	ABall* ball = Cast<ABall>(OtherActor);
	if (ball && bIsInteracted && !flipperDestination.Equals(flipperMesh->GetComponentRotation(), 2))
	{
		ball->GetSphereMeshComponent()->AddImpulse(FVector(0, 0, BALL_IMPULSE));
	}
}