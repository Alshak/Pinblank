// Copyright Alain SHAKOUR 2016 contact@alainshakour.com This software is a computer program whose purpose is entertainment.This software is governed by the CeCILL-C license under French law and abiding by the rules of distribution of free software.  You can  use, modify and/ or redistribute the software under the terms of the CeCILL-C license as circulated by CEA, CNRS and INRIA at the following URL "http://www.cecill.info". As a counterpart to the access to the source code and  rights to copy, modify and redistribute granted by the license, users are provided only with a limited warranty  and the software's author,  the holder of the economic rights,  and the successive licensors  have only  limited liability. In this respect, the user's attention is drawn to the risks associated with loading,  using,  modifying and/or developing or reproducing the software by the user in light of its specific status of free software, that may mean  that it is complicated to manipulate,  and  that  also therefore means  that it is reserved for developers  and  experienced professionals having in-depth computer knowledge. Users are therefore encouraged to load and test the software's suitability as regards their requirements in conditions enabling the security of their systems and/or data to be ensured and,  more generally, to use and operate it in the same conditions as regards security. The fact that you are presently reading this means that you have had knowledge of the CeCILL-C license and that you accept its terms.

#include "Pinblank.h"
#include "Ball.h"
#include "Expeditor.h"

AExpeditor::AExpeditor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Cube mesh
	cubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("cubeMesh"));
	RootComponent = cubeMesh;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> cubeVisualAsset(TEXT("/Game/Meshes/Shape_Sphere.Shape_Sphere"));
	if (cubeVisualAsset.Succeeded())
	{
		cubeMesh->SetStaticMesh(cubeVisualAsset.Object);
		static ConstructorHelpers::FObjectFinder<UMaterial> materialResource(TEXT("/Game/Materials/M_WallShader_Master.M_WallShader_Master"));
		if (materialResource.Succeeded())
		{
			cubeMesh->SetMaterial(0, materialResource.Object);
		}
	}
	cubeMesh->SetWorldScale3D(FVector(0.2f));
	cubeMesh->SetNotifyRigidBodyCollision(true);
	cubeMesh->SetMobility(EComponentMobility::Static);
	this->OnActorHit.AddDynamic(this, &AExpeditor::OnHitActor);

	// Particle System
	particleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particles"));
	particleSystem->AttachTo(cubeMesh);
	particleSystem->SetRelativeRotation(FRotator(90, 0, 0));
	particleSystem->SetRelativeScale3D(FVector(5));
	particleSystem->bAutoActivate = false;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> particleAsset(TEXT("/Game/Particles/PFX_Param_Explosion.PFX_Param_Explosion"));
	if (particleAsset.Succeeded())
	{
		particleSystem->SetTemplate(particleAsset.Object);
	}
}

// Called when the game starts or when spawned
void AExpeditor::BeginPlay()
{
	Super::BeginPlay();
	ChangeColor(meshColor);	
}

void AExpeditor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Assign dynamic material to allow color change
	if (GetColoredMesh() != nullptr && GetColoredMesh()->GetMaterial(0)) {
		dynamicMaterialInstance = UMaterialInstanceDynamic::Create(GetColoredMesh()->GetMaterial(0), this);
		GetColoredMesh()->SetMaterial(0, dynamicMaterialInstance);
	}
}

UStaticMeshComponent* AExpeditor::GetColoredMesh()
{
	return cubeMesh;
}

void AExpeditor::ChangeColor(FLinearColor color)
{
	dynamicMaterialInstance->SetVectorParameterValue(TEXT("Wall Color"), color);
}

void AExpeditor::OnHitActor(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	ABall* ball = Cast<ABall>(OtherActor);
	if (ball)
	{
		// Add impulse to the ball
		FVector normal = Hit.Normal;
		ball->GetSphereMeshComponent()->AddImpulse(normal*-BALL_IMPULSE);

		// Activate particle system
		if (particleSystem && particleSystem->Template)
		{
			particleSystem->ToggleActive();
		}

		// Decrease lives. If life = 0, actor is destroyed. If life < 0, actor is invulnerable.
		if (NbLives > 0)
		{
			NbLives--;
			if (NbLives == 0) {
				this->Destroy();
			}
		}
	}
}
