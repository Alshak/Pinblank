// Copyright Alain SHAKOUR 2016 contact@alainshakour.com This software is a computer program whose purpose is entertainment.This software is governed by the CeCILL-C license under French law and abiding by the rules of distribution of free software.  You can  use, modify and/ or redistribute the software under the terms of the CeCILL-C license as circulated by CEA, CNRS and INRIA at the following URL "http://www.cecill.info". As a counterpart to the access to the source code and  rights to copy, modify and redistribute granted by the license, users are provided only with a limited warranty  and the software's author,  the holder of the economic rights,  and the successive licensors  have only  limited liability. In this respect, the user's attention is drawn to the risks associated with loading,  using,  modifying and/or developing or reproducing the software by the user in light of its specific status of free software, that may mean  that it is complicated to manipulate,  and  that  also therefore means  that it is reserved for developers  and  experienced professionals having in-depth computer knowledge. Users are therefore encouraged to load and test the software's suitability as regards their requirements in conditions enabling the security of their systems and/or data to be ensured and,  more generally, to use and operate it in the same conditions as regards security. The fact that you are presently reading this means that you have had knowledge of the CeCILL-C license and that you accept its terms.

#include "Pinblank.h"
#include "Ball.h"
#include "DynamicCamera.h"

// Sets default values
ADynamicCamera::ADynamicCamera()
{
	PrimaryActorTick.bCanEverTick = true;
	bBallHasLeft = false;

	// Collider which always include the ball
	UBoxComponent* boxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	RootComponent = boxCollider;
	boxCollider->SetWorldScale3D(FVector(4,8,8));
	boxCollider->bGenerateOverlapEvents = true;
	boxCollider->OnComponentEndOverlap.AddDynamic(this, &ADynamicCamera::OnEndOverlap);

	// Camera
	UCameraComponent* cameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	cameraComponent->AttachTo(boxCollider);
	cameraComponent->SetRelativeLocation(FVector(CAMERA_DISTANCE,0,0));

}

void ADynamicCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADynamicCamera::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	// If the ball wants to left the box collider, we move the box collider with the camera to the position of the ball
	if (bBallHasLeft) {
		FVector res = FMath::VInterpTo(this->GetActorLocation(), ballLastPosition, DeltaTime, 6);
		this->SetActorLocation(res);
		// We stop moving if we are in the right place
		if (ballLastPosition.Equals(this->GetActorLocation(), 0.1))
		{
			bBallHasLeft = false;
		}
	}
}

void ADynamicCamera::OnEndOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABall* ball = Cast<ABall>(OtherActor);
	if (ball)
	{
		ballLastPosition = ball->GetActorLocation();
		bBallHasLeft = true;
	}
}
