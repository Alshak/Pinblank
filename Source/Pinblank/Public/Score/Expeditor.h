// Copyright Alain SHAKOUR 2016 contact@alainshakour.com This software is a computer program whose purpose is entertainment.This software is governed by the CeCILL-C license under French law and abiding by the rules of distribution of free software.  You can  use, modify and/ or redistribute the software under the terms of the CeCILL-C license as circulated by CEA, CNRS and INRIA at the following URL "http://www.cecill.info". As a counterpart to the access to the source code and  rights to copy, modify and redistribute granted by the license, users are provided only with a limited warranty  and the software's author,  the holder of the economic rights,  and the successive licensors  have only  limited liability. In this respect, the user's attention is drawn to the risks associated with loading,  using,  modifying and/or developing or reproducing the software by the user in light of its specific status of free software, that may mean  that it is complicated to manipulate,  and  that  also therefore means  that it is reserved for developers  and  experienced professionals having in-depth computer knowledge. Users are therefore encouraged to load and test the software's suitability as regards their requirements in conditions enabling the security of their systems and/or data to be ensured and,  more generally, to use and operate it in the same conditions as regards security. The fact that you are presently reading this means that you have had knowledge of the CeCILL-C license and that you accept its terms.

#pragma once

#include "GameFramework/Actor.h"
#include "ColorChangeable.h"
#include "Expeditor.generated.h"

UCLASS()
class PINBLANK_API AExpeditor : public AActor, public IColorChangeable
{
	GENERATED_BODY()

	const int BALL_IMPULSE = 50;
	bool gravityY = false;
	int speed = 80;
	bool isUpImpulse = false;
	UParticleSystem* particleSystem;
	UMaterialInstanceDynamic* dynamicMaterialInstance;

	AExpeditor();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostInitializeComponents() override;

	// Color change impl
	virtual UStaticMeshComponent* GetColoredMesh() override;
	virtual const FName GetMaterialParameterColorName() const override;

	UFUNCTION()
		void OnHitActor(UPrimitiveComponent* ComponentHit, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
public:	
	
	void EnableYGravity(int speed, bool pIsUpImpulse = true);

	UPROPERTY(EditAnywhere)
		int NbLives = 1;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* sphereMesh;

	UPROPERTY(EditAnywhere)
		FLinearColor meshColor = FLinearColor(1, 0, 0);
};
