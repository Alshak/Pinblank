// Copyright Alain SHAKOUR 2016 contact@alainshakour.com This software is a computer program whose purpose is entertainment.This software is governed by the CeCILL-C license under French law and abiding by the rules of distribution of free software.  You can  use, modify and/ or redistribute the software under the terms of the CeCILL-C license as circulated by CEA, CNRS and INRIA at the following URL "http://www.cecill.info". As a counterpart to the access to the source code and  rights to copy, modify and redistribute granted by the license, users are provided only with a limited warranty  and the software's author,  the holder of the economic rights,  and the successive licensors  have only  limited liability. In this respect, the user's attention is drawn to the risks associated with loading,  using,  modifying and/or developing or reproducing the software by the user in light of its specific status of free software, that may mean  that it is complicated to manipulate,  and  that  also therefore means  that it is reserved for developers  and  experienced professionals having in-depth computer knowledge. Users are therefore encouraged to load and test the software's suitability as regards their requirements in conditions enabling the security of their systems and/or data to be ensured and,  more generally, to use and operate it in the same conditions as regards security. The fact that you are presently reading this means that you have had knowledge of the CeCILL-C license and that you accept its terms.

#include "Pinblank.h"
#include "Spawner.h"
#include "Expeditor.h"

// Sets default values
ASpawner::ASpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpawner::BeginPlay()
{
	Super::BeginPlay();
	FTimerHandle uniqueHandle;
	FTimerDelegate spawnDelegate = FTimerDelegate::CreateUObject(this, &ASpawner::SpawnExpeditor);
	GetWorldTimerManager().SetTimer(uniqueHandle, spawnDelegate, frequency, true);
}

void ASpawner::SpawnExpeditor()
{
	AExpeditor* expeditor = GetWorld()->SpawnActor<AExpeditor>(AExpeditor::StaticClass(),GetActorLocation(),FRotator::ZeroRotator);
	if (expeditor) {
		expeditor->EnableYGravity(40);
		expeditor->NbLives = 1;
		expeditor->ChangeColor(FColor::Orange);
	}
}

// Called every frame
void ASpawner::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

