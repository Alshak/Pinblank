// Copyright Alain SHAKOUR 2016 contact@alainshakour.com This software is a computer program whose purpose is entertainment.This software is governed by the CeCILL-C license under French law and abiding by the rules of distribution of free software.  You can  use, modify and/ or redistribute the software under the terms of the CeCILL-C license as circulated by CEA, CNRS and INRIA at the following URL "http://www.cecill.info". As a counterpart to the access to the source code and  rights to copy, modify and redistribute granted by the license, users are provided only with a limited warranty  and the software's author,  the holder of the economic rights,  and the successive licensors  have only  limited liability. In this respect, the user's attention is drawn to the risks associated with loading,  using,  modifying and/or developing or reproducing the software by the user in light of its specific status of free software, that may mean  that it is complicated to manipulate,  and  that  also therefore means  that it is reserved for developers  and  experienced professionals having in-depth computer knowledge. Users are therefore encouraged to load and test the software's suitability as regards their requirements in conditions enabling the security of their systems and/or data to be ensured and,  more generally, to use and operate it in the same conditions as regards security. The fact that you are presently reading this means that you have had knowledge of the CeCILL-C license and that you accept its terms.

#include "Pinblank.h"
#include "FlipperActionable.h"
#include "Ball.h"

// Sets default values
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
	sphereMesh->SetWorldScale3D(FVector(0.25f));
	sphereMesh->SetSimulatePhysics(true);
	sphereMesh->SetEnableGravity(true);

	// Sphere collider to detect FlipperActionable
	sphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	sphereCollider->AttachTo(sphereMesh);
	sphereCollider->InitSphereRadius(SPHERE_RADIUS);
	sphereCollider->SetRelativeScale3D(FVector(4.f));
	sphereCollider->SetRelativeLocation(FVector(0, 0, 50));
	sphereCollider->bGenerateOverlapEvents = true;
	sphereCollider->OnComponentBeginOverlap.AddDynamic(this, &ABall::OnBeginOverlap);
	sphereCollider->OnComponentEndOverlap.AddDynamic(this, &ABall::OnEndOverlap);

	// Physical material to remove bouciness
	const ConstructorHelpers::FObjectFinder<UPhysicalMaterial> physMat(TEXT("/Game/PhysicalMaterials/Ball"));
	if (GEngine) {
		sphereMesh->SetPhysMaterialOverride(physMat.Object);
	}
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ABall::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAction("FlipperAction", IE_Pressed, this, &ABall::FlipperAction);
	InputComponent->BindAction("FlipperAction", IE_Released, this, &ABall::FlipperStopAction);
}

// Called when the game starts or when spawned
void ABall::BeginPlay()
{
	Super::BeginPlay();
}

void ABall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Limit ball velocity to size 900
	sphereMesh->SetPhysicsLinearVelocity(sphereMesh->GetComponentVelocity().GetClampedToMaxSize(900));
}

void ABall::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

// Interact with FlipperActionable
void ABall::FlipperAction()
{
	TArray<AActor *> flippers;
	sphereCollider->GetOverlappingActors(flippers);
	for (auto flip : flippers)
	{
		AFlipperActionable* actionableActor = Cast<AFlipperActionable>(flip);
		if (actionableActor)
		{
			actionableActor->Interact(this);
			bIsInteracted = true;
			actionableActor->ChangeColor(FLinearColor(0, 1, 0));
		}
	}
}

// Stop interacting with FlipperActionable
void ABall::FlipperStopAction()
{
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AFlipperActionable* actionableActor = Cast<AFlipperActionable>(*It);
		if (actionableActor)
		{
			bIsInteracted = false;
			actionableActor->StopInteract(this);
		}
	}
}

UStaticMeshComponent* ABall::GetSphereMeshComponent()
{
	return sphereMesh;
}

void ABall::OnBeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFlipperActionable* actionableActor = Cast<AFlipperActionable>(OtherActor);
	if (actionableActor)
	{
		// If button already press, interact with the new component
		if (bIsInteracted) {
			FlipperAction();
		}
		// Change color of interactable objects
		actionableActor->ChangeColor(FLinearColor(0, 1, 0));
	}
}

void ABall::OnEndOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AFlipperActionable* actionableActor = Cast<AFlipperActionable>(OtherActor);
	if (actionableActor)
	{
		// Stop changing color of interactable objects
		actionableActor->ChangeColor(FLinearColor(1, 0, 0));
	}
}