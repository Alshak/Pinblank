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
	flipperMesh->SetWorldScale3D(FVector(0.026f));
	this->SetActorRotation(FRotator(0, 0, 0));
	flipperMesh->SetEnableGravity(false);
	flipperMesh->SetSimulatePhysics(false);
	flipperMesh->SetNotifyRigidBodyCollision(true);
	flipperMesh->OnComponentHit.AddDynamic(this, &AFlipper::OnHitActor);

	// Collider to detect ball position
	UCapsuleComponent* capsuleCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	capsuleCollider->AttachTo(RootComponent);
	capsuleCollider->SetCapsuleSize(CAPSULE_RADIUS, CAPSULE_HALF_HEIGHT);
	capsuleCollider->SetRelativeScale3D(FVector(60));
	capsuleCollider->SetRelativeRotation(FRotator(0, 0, 90));
	capsuleCollider->SetEnableGravity(false);
}

void AFlipper::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	IColorChangeable::InitializeDynamicMaterialInstance(this);
	ChangeColor(FLinearColor(1, 0, 0));

	// Register the current Z rotation to allow rotation on this axis in the editor
	currentYaw = flipperMesh->GetComponentRotation().Pitch;
	flipperDestination = FRotator(currentYaw, 0, bottomAngle);
}


void AFlipper::BeginPlay()
{
	Super::BeginPlay();	
}

void AFlipper::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	// Rotate the flipper
	if (hasNewDestination)
	{
		flipperDestination = FRotator(flipperMesh->GetComponentRotation().Pitch, rotDestination, flipperMesh->GetComponentRotation().Roll);
		flipperMesh->SetWorldRotation(FMath::RInterpConstantTo(flipperMesh->GetComponentRotation(), flipperDestination, DeltaTime, rotationInterpSpeed));
		if (flipperDestination.Equals(flipperMesh->GetComponentRotation(), 0.1))
		{
			hasNewDestination = false;
		}
	}
}

void AFlipper::StartFirstInteraction(ABall* ball)
{
	bIsInteracted = true;
	hasNewDestination = true;
	rotDestination = topAngle;
}

void AFlipper::StopFirstInteraction(ABall* ball)
{
	bIsInteracted = false;
	hasNewDestination = true;
	rotDestination = bottomAngle;
}

void AFlipper::StartSecondInteraction(ABall* ball)
{
}

void AFlipper::StopSecondInteraction(ABall* ball)
{
}

UStaticMeshComponent* AFlipper::GetColoredMesh()
{
	return flipperMesh;
}

const FName AFlipper::GetMaterialParameterColorName() const
{
	return TEXT("ParamColor");
}

void AFlipper::OnHitActor(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Add some impulse on ball hit
	if (BALL_IMPULSE > 0) {
		ABall* ball = Cast<ABall>(OtherActor);

		if (ball && bIsInteracted && !flipperDestination.Equals(flipperMesh->GetComponentRotation(), 2)) {
			FVector localImpact = this->GetTransform().InverseTransformPosition(Hit.ImpactPoint);
			if (localImpact.Y < 80) {
				ball->AddSphereImpulse(this, FVector(0, -BALL_IMPULSE, 0));
			}
		}
	}
}
