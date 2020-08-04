// Copyright Eray Kalkan 2020

#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "Grabber.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandle();
	SetupInputComponent();
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!PhysicsHandle)
	{
		return;
	}
	if (PhysicsHandle->GrabbedComponent)
	{
		// Move the object we are holding
		PhysicsHandle->SetTargetLocation(GetPlayersReach());
	}
}

// Checking for physics handle component
void UGrabber::FindPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("The object %s does not have PhysicsHandle attached to it."), *GetOwner()->GetName());

	}
}

void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("InputComponent found on: %s"), *GetOwner()->GetName());
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InputComponent missing on: %s"), *GetOwner()->GetName());
	}
}

void UGrabber::Grab()
{
	// only raycast when key is pressed and see if we reach any actors with physics body collision channel set.
	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	
	// If we hit something then attach physics handle
	//TODO attach physcis handle

	AActor* ActorHit = HitResult.GetActor();

	if (ActorHit)
	{
		if (!PhysicsHandle)
		{
			return;
		}
		PhysicsHandle->GrabComponentAtLocation(
			ComponentToGrab,
			NAME_None,
			GetPlayersReach()
		);
	}

}

void UGrabber::Release()
{
	if (!PhysicsHandle)
	{
		return;
	}
	//TODO remove/release physics handle
	PhysicsHandle->ReleaseComponent();
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	//UE_LOG(LogTemp, Warning, TEXT("Location: %s , Rotation: %s"), *PlayerViewPointLocation.ToString(), *PlayerViewPointRotation.ToString());

	// Draw a Line fom player showing the reach.
	/*DrawDebugLine(
		GetWorld(),
		PlayerViewPointLocation,
		LineTraceEnd,
		FColor(0, 255, 0),
		false,
		0.f,
		0,
		5.f
	);*/

	FHitResult Hit; //OUT PARAMETER
	// Ray-cast out to a certain distance (Reach)
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner()); // ignore itself

	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		GetPlayersWorldPos(),
		GetPlayersReach(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	// Logging to test

	AActor* ActorHit = Hit.GetActor();
	if (ActorHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("The actor %s has been hit"), *ActorHit->GetName());
	}

	return Hit;

}
FVector UGrabber::GetPlayersReach() const
{
	PlayerLocationRotation LocationRotation = GetPlayerLocationRotation();

	return LocationRotation.PlayerViewPointLocation + LocationRotation.PlayerViewPointRotation.Vector() * Reach;
}

FVector UGrabber::GetPlayersWorldPos() const
{
	return GetPlayerLocationRotation().PlayerViewPointLocation;
}

PlayerLocationRotation UGrabber::GetPlayerLocationRotation() const
{
	FVector PlayerViewPointLocation; //OUT PARAMETER
	FRotator PlayerViewPointRotation; //OUT PARAMETER

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	PlayerLocationRotation LocationRotation;
	LocationRotation.PlayerViewPointLocation = PlayerViewPointLocation;
	LocationRotation.PlayerViewPointRotation = PlayerViewPointRotation;

	return LocationRotation;
}