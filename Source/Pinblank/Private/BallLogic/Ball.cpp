// Copyright Alain SHAKOUR 2016 contact@alainshakour.com This software is a computer program whose purpose is entertainment.This software is governed by the CeCILL-C license under French law and abiding by the rules of distribution of free software.  You can  use, modify and/ or redistribute the software under the terms of the CeCILL-C license as circulated by CEA, CNRS and INRIA at the following URL "http://www.cecill.info". As a counterpart to the access to the source code and  rights to copy, modify and redistribute granted by the license, users are provided only with a limited warranty  and the software's author,  the holder of the economic rights,  and the successive licensors  have only  limited liability. In this respect, the user's attention is drawn to the risks associated with loading,  using,  modifying and/or developing or reproducing the software by the user in light of its specific status of free software, that may mean  that it is complicated to manipulate,  and  that  also therefore means  that it is reserved for developers  and  experienced professionals having in-depth computer knowledge. Users are therefore encouraged to load and test the software's suitability as regards their requirements in conditions enabling the security of their systems and/or data to be ensured and,  more generally, to use and operate it in the same conditions as regards security. The fact that you are presently reading this means that you have had knowledge of the CeCILL-C license and that you accept its terms.

#include "Pinblank.h"
#include "FlipperActionable.h"
#include "ColorChangeable.h"
#include "Ball.h"

ABall::ABall()
{
	PrimaryActorTick.bCanEverTick = true;
	// Sphere mesh
	sphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereVisualRepresentation"));
	RootComponent = sphereMesh;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> sphereVisualAsset(TEXT("/Game/Meshes/Shape_Sphere.Shape_Sphere"));
	if (sphereVisualAsset.Succeeded())
	{
		sphereMesh->SetStaticMesh(sphereVisualAsset.Object);
		static ConstructorHelpers::FObjectFinder<UMaterial> materialResource(TEXT("/Game/Materials/M_PlayerRim.M_PlayerRim"));
		if (materialResource.Succeeded())
		{
			sphereMesh->SetMaterial(0, materialResource.Object);
		}
	}
	sphereMesh->SetWorldScale3D(FVector(0.08f));

	// Sphere collider to detect FlipperActionable
	sphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	sphereCollider->AttachTo(sphereMesh);
	sphereCollider->InitSphereRadius(SPHERE_RADIUS);
	sphereCollider->SetRelativeScale3D(FVector(4.f));
	sphereCollider->SetRelativeLocation(FVector(0, 0, 50));
	sphereCollider->bGenerateOverlapEvents = true;
	sphereCollider->OnComponentBeginOverlap.AddDynamic(this, &ABall::OnBeginOverlap);
	sphereCollider->OnComponentEndOverlap.AddDynamic(this, &ABall::OnEndOverlap);

	// Physical material to remove bounciness
	const ConstructorHelpers::FObjectFinder<UPhysicalMaterial> physMat(TEXT("/Game/PhysicalMaterials/PM_Ball"));
	if (GEngine) {
		sphereMesh->SetPhysMaterialOverride(physMat.Object);
	}

	sphereMesh->BodyInstance.SetDOFLock(EDOFMode::XYPlane);
}

void ABall::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAction("FlipperFirstAction", IE_Pressed, this, &ABall::FlipperStartFirstAction);
	InputComponent->BindAction("FlipperFirstAction", IE_Released, this, &ABall::FlipperStopFirstAction);
	InputComponent->BindAction("FlipperSecondAction", IE_Pressed, this, &ABall::FlipperStartSecondAction);
	InputComponent->BindAction("FlipperSecondAction", IE_Released, this, &ABall::FlipperStopSecondAction);
}

void ABall::BeginPlay()
{
	Super::BeginPlay();

	//Change color of actionables already overlapping
	TArray<AActor *> flippers;
	sphereCollider->GetOverlappingActors(flippers);
	for (auto flip : flippers)
	{
		ChangeActionableColor(flip, FLinearColor(0, 1, 0));
	}

	if (isPhysicsActivated)
	{
		ActivatePhysics();
	}
}

void ABall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (sphereMesh->IsSimulatingPhysics()) {
		//Set max speed
		sphereMesh->SetPhysicsLinearVelocity(sphereMesh->GetComponentVelocity().GetClampedToMaxSize(maxSpeed));
		sphereMesh->AddForce(FVector(0, 80, 0));
	}
}

void ABall::ActivatePhysics()
{
	sphereMesh->SetEnableGravity(true);
	sphereMesh->SetSimulatePhysics(true);
}

void ABall::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void ABall::FlipperStartFirstAction()
{
	TArray<AActor *> flippers;
	sphereCollider->GetOverlappingActors(flippers);
	for (auto flip : flippers)
	{
		// Interact with Actionable
		IFlipperActionable* actionableActor = Cast<IFlipperActionable>(flip);
		if (actionableActor)
		{
			actionableActor->StartFirstInteraction(this);
			bIsInteracted = true;
		}
	}
}

void ABall::FlipperStopFirstAction()
{
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		// Stop interacting with Actionable
		IFlipperActionable* actionableActor = Cast<IFlipperActionable>(*It);
		if (actionableActor)
		{
			bIsInteracted = false;
			actionableActor->StopFirstInteraction(this);
		}
	}
}

void ABall::FlipperStartSecondAction()
{
	TArray<AActor *> flippers;
	sphereCollider->GetOverlappingActors(flippers);
	for (auto flip : flippers)
	{
		// Interact with Actionable
		IFlipperActionable* actionableActor = Cast<IFlipperActionable>(flip);
		if (actionableActor)
		{
			actionableActor->StartSecondInteraction(this);
			bIsInteracted = true;
		}
	}
}

void ABall::FlipperStopSecondAction()
{
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		// Stop interacting with Actionable
		IFlipperActionable* actionableActor = Cast<IFlipperActionable>(*It);
		if (actionableActor)
		{
			bIsInteracted = false;
			actionableActor->StopSecondInteraction(this);
		}
	}
}

const UStaticMeshComponent* ABall::GetSphereMeshComponent() const
{
	return sphereMesh;
}

void ABall::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ChangeActionableColor(OtherActor, FLinearColor(0, 1, 0));
}

void ABall::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// If ball use physics (e.g. add impulse), it emits this event. So we verify if the collider and the overlap each other before continuing.
	if(!sphereCollider->IsOverlappingActor(OtherActor))
		ChangeActionableColor(OtherActor, FLinearColor(1, 0, 0));
}

void ABall::AddSphereImpulse(AActor* OtherActor, FVector force)
{
	if (sphereMesh->ComponentVelocity.Z <= 0)
	{
		sphereMesh->AddImpulse(force);
	}
}

void ABall::ChangeActionableColor(AActor* Actor, FLinearColor color) {
	IFlipperActionable* actionableActor = Cast<IFlipperActionable>(Actor);
	if (actionableActor)
	{
		IColorChangeable* colorChangeableActor = Cast<IColorChangeable>(actionableActor);
		if (colorChangeableActor)
		{
			colorChangeableActor->ChangeColor(color);
		}
	}
}
