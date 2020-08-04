// Copyright Eray Kalkan 2020

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "OpenDoor.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Components/AudioComponent.h"

#define OUT
// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	InitialYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	OpenAngle += InitialYaw;

	FindPressurePlate();
	//ActorThatOpens = GetWorld()->GetFirstPlayerController()->GetPawn();

	FindAudioComponent();
	
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PressurePlate && TotalMassOfActors() > MassRequiredToOpenDoor)
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}
	else
	{
		if (GetWorld()->GetTimeSeconds() - DoorLastOpened > DoorCloseDelay)
		{
			CloseDoor(DeltaTime);
		}
	}
	
	/*CurrentYaw = FMath::FInterpTo(CurrentYaw, OpenAngle, DeltaTime, 2);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);*/

	/*float CurrentYaw = GetOwner()->GetActorRotation().Yaw;
	FRotator OpenDoor(0.f, OpenAngle, 0.f);*/

	//OpenDoor.Yaw = FMath::Lerp(CurrentYaw, OpenAngle, 0.02f);
	//OpenDoor.Yaw = FMath::FInterpConstantTo(CurrentYaw, OpenAngle, DeltaTime, 45); //45 = 2 seconds to reach 90 degrees 90/2=45
	
	/*OpenDoor.Yaw = FMath::FInterpTo(CurrentYaw, OpenAngle, DeltaTime, 2); //for smoother interpolation
	GetOwner()->SetActorRotation(OpenDoor);*/

	//FRotator CurrentRotation =  GetOwner()->GetActorRotation();
	//CurrentRotation.Yaw = 90.f;

	//FRotator OpenDoorRotation(0.f,90.f,0.f);

	//GetOwner()->SetActorRotation(OpenDoorRotation);

}

void UOpenDoor::OpenDoor(float DeltaTime)
{
	CurrentYaw = FMath::FInterpTo(CurrentYaw, OpenAngle, DeltaTime, DoorOpenSpeed);
	//CurrentYaw = FMath::Lerp(CurrentYaw, OpenAngle, DeltaTime * 1.f);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);

	bCloseDoorSoundPlayed = false;
	if (AudioComponent && !bOpenDoorSoundPlayed)
	{
		AudioComponent->Play();
		bOpenDoorSoundPlayed = true;
	}

}

void UOpenDoor::CloseDoor(float DeltaTime)
{
	CurrentYaw = FMath::FInterpTo(CurrentYaw, InitialYaw, DeltaTime, DoorCloseSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);

	bOpenDoorSoundPlayed = false;
	if (AudioComponent && !bCloseDoorSoundPlayed)
	{
		AudioComponent->Play();
		bCloseDoorSoundPlayed = true;
	}
		
}

float UOpenDoor::TotalMassOfActors() const
{

	float TotalMass = 0.f;

	if (!PressurePlate)
	{
		return TotalMass;
	}

	//Find all overlapping actors
	TArray<AActor*> OverlappingActors;
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);

	//add up their masses
	for (AActor* Actor:OverlappingActors)
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}

	return TotalMass;
}

void UOpenDoor::FindPressurePlate()
{
	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("The actor %s has the opendoor component on it but no pressureplate set."), *GetOwner()->GetName());
	}

}

void UOpenDoor::FindAudioComponent() 
{
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();
	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing audio component"), *GetOwner()->GetName());
	}
}