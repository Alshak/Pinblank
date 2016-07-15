// Copyright Alain SHAKOUR 2016 contact@alainshakour.com This software is a computer program whose purpose is entertainment.This software is governed by the CeCILL-C license under French law and abiding by the rules of distribution of free software.  You can  use, modify and/ or redistribute the software under the terms of the CeCILL-C license as circulated by CEA, CNRS and INRIA at the following URL "http://www.cecill.info". As a counterpart to the access to the source code and  rights to copy, modify and redistribute granted by the license, users are provided only with a limited warranty  and the software's author,  the holder of the economic rights,  and the successive licensors  have only  limited liability. In this respect, the user's attention is drawn to the risks associated with loading,  using,  modifying and/or developing or reproducing the software by the user in light of its specific status of free software, that may mean  that it is complicated to manipulate,  and  that  also therefore means  that it is reserved for developers  and  experienced professionals having in-depth computer knowledge. Users are therefore encouraged to load and test the software's suitability as regards their requirements in conditions enabling the security of their systems and/or data to be ensured and,  more generally, to use and operate it in the same conditions as regards security. The fact that you are presently reading this means that you have had knowledge of the CeCILL-C license and that you accept its terms.

#include "Pinblank.h"
#include "FollowingPaddle.h"

AFollowingPaddle::AFollowingPaddle()
{
	PrimaryActorTick.bCanEverTick = true;
	// Flipper mesh
	boxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMesh"));
	RootComponent = boxMesh;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> paddleVisualAsset(TEXT("/Game/Meshes/Shape_NarrowCapsule.Shape_NarrowCapsule"));
	if (paddleVisualAsset.Succeeded())
	{
		boxMesh->SetStaticMesh(paddleVisualAsset.Object);
		static ConstructorHelpers::FObjectFinder<UMaterial> MaterialResource(TEXT("/Game/Materials/M_MasterEnemy.M_MasterEnemy"));
		if (MaterialResource.Succeeded())
		{
			boxMesh->SetMaterial(0, MaterialResource.Object);
		}
	}
	boxMesh->SetWorldRotation(FRotator(0, 0, 90));
	boxMesh->SetWorldScale3D(FVector(0.16,0.16,0.3));
	boxMesh->SetEnableGravity(false);
	boxMesh->SetSimulatePhysics(false);
	boxMesh->SetNotifyRigidBodyCollision(true);
	boxMesh->OnComponentHit.AddDynamic(this, &AFollowingPaddle::OnHitActor);

	// Collider to detect ball position
	UBoxComponent* boxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	boxCollider->AttachTo(RootComponent);
	boxCollider->SetRelativeScale3D(FVector(25,25,5));
	boxCollider->SetEnableGravity(false);

}

void AFollowingPaddle::BeginPlay()
{
	Super::BeginPlay();

}

void AFollowingPaddle::Tick(float DeltaSeconds)
{
	if (boxMesh) {
		if (isFirstActionHolded) {
			boxMesh->SetWorldLocation(FMath::VInterpConstantTo(boxMesh->GetComponentLocation(), secondInteractionDestination, DeltaSeconds, paddleSpeed));
		}
		else {
			boxMesh->SetWorldLocation(FMath::VInterpConstantTo(boxMesh->GetComponentLocation(), firstInteractionDestination, DeltaSeconds, paddleSpeed));
		}
	}
}
void AFollowingPaddle::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	IColorChangeable::InitializeDynamicMaterialInstance(this);
	if (firstInteractionDestinationActor) {
		firstInteractionDestination = firstInteractionDestinationActor->GetActorLocation();
	}
	else {
		firstInteractionDestination = boxMesh->GetComponentLocation();
	}
	if(secondInteractionDestinationActor)
		secondInteractionDestination = secondInteractionDestinationActor->GetActorLocation();
}

void AFollowingPaddle::StartFirstInteraction(ABall* ball) {

	isFirstActionHolded = true;
}

void AFollowingPaddle::StopFirstInteraction(ABall* ball) {
	isFirstActionHolded = false;
}

void AFollowingPaddle::StartSecondInteraction(ABall* ball)
{
	isSecondActionHolded = true;
}

void AFollowingPaddle::StopSecondInteraction(ABall* ball)
{
	isSecondActionHolded = false;
}
UStaticMeshComponent* AFollowingPaddle::GetColoredMesh()
{
	return boxMesh;
}

const FName AFollowingPaddle::GetMaterialParameterColorName() const
{
	return TEXT("ParamColor");
}

void AFollowingPaddle::OnHitActor(UPrimitiveComponent* ComponentHit, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Add some impulse on ball hit
	if (ballImpulse > 0) {
		ABall* ball = Cast<ABall>(OtherActor);
		if (ball)
		{
			FVector offset = FVector::ZeroVector;
			FVector localImpact = this->GetTransform().InverseTransformPosition(Hit.ImpactPoint);
			if (localImpact.Z > 50)
			{
				offset = FVector(-OFFSET_DEVIATION_Y, 0, 0);
			}
			else if (localImpact.Z < 50) {
				offset = FVector(OFFSET_DEVIATION_Y, 0, 0);
			}
			ball->AddSphereImpulse(this, FVector(0, -ballImpulse, 0) + offset);
		}
	}
}
