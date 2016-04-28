// Copyright Alain SHAKOUR 2016 contact@alainshakour.com This software is a computer program whose purpose is entertainment.This software is governed by the CeCILL-C license under French law and abiding by the rules of distribution of free software.  You can  use, modify and/ or redistribute the software under the terms of the CeCILL-C license as circulated by CEA, CNRS and INRIA at the following URL "http://www.cecill.info". As a counterpart to the access to the source code and  rights to copy, modify and redistribute granted by the license, users are provided only with a limited warranty  and the software's author,  the holder of the economic rights,  and the successive licensors  have only  limited liability. In this respect, the user's attention is drawn to the risks associated with loading,  using,  modifying and/or developing or reproducing the software by the user in light of its specific status of free software, that may mean  that it is complicated to manipulate,  and  that  also therefore means  that it is reserved for developers  and  experienced professionals having in-depth computer knowledge. Users are therefore encouraged to load and test the software's suitability as regards their requirements in conditions enabling the security of their systems and/or data to be ensured and,  more generally, to use and operate it in the same conditions as regards security. The fact that you are presently reading this means that you have had knowledge of the CeCILL-C license and that you accept its terms.

#pragma once

#include "GameFramework/Actor.h"
#include "FlipperActionable.h"
#include "Flipper.generated.h"

UCLASS()
class PINBLANK_API AFlipper : public AFlipperActionable
{
	GENERATED_BODY()

	FVector initialPosition;
	FRotator flipperDestination;

	float currentYaw;
	// Small optimization
	bool hasNewDestination = false;

	const int BALL_IMPULSE = 1500;
	const int CAPSULE_RADIUS = 25;
	const int CAPSULE_HALF_HEIGHT = 44;
	bool bIsInteracted;
public:
	AFlipper();
	void BeginPlay() override;
	void Tick( float DeltaSeconds ) override;
	void PostInitializeComponents() override;

	// Flipper action impl
	void Interact(ABall* ball) override;
	void StopInteract(ABall* ball) override;
	virtual UStaticMeshComponent* GetColoredMesh() override;

	UFUNCTION()
	void OnHitActor(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit) ;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* flipperMesh;

	UPROPERTY(EditAnywhere)
	int topAngle = 60;

	UPROPERTY(EditAnywhere)
	int bottomAngle = 120;

	UPROPERTY(EditAnywhere)
	int rotationInterpSpeed = 250;
};
