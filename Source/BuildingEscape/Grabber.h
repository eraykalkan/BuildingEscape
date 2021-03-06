// Copyright Eray Kalkan 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Grabber.generated.h"

struct PlayerLocationRotation
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;


private:
	float Reach = 100.f; // 1 meter

	UPROPERTY()
	UPhysicsHandleComponent* PhysicsHandle = nullptr;

	UPROPERTY()
	UInputComponent* InputComponent = nullptr;

	void Grab();
	void Release();
	void FindPhysicsHandle();
	void SetupInputComponent();

	//Return the first actor within reach with physics body.
	FHitResult GetFirstPhysicsBodyInReach() const;

	//Return the LineTraceEnd
	FVector GetPlayersReach() const;

	//Get Players Location in the world
	FVector GetPlayersWorldPos() const;

	//Get PlayerLocationRotation struct
	PlayerLocationRotation GetPlayerLocationRotation() const;
};