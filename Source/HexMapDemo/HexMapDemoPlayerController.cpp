// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "HexMapDemoPlayerController.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "HexMapDemoCharacter.h"
#include "HMGrid.h"
#include "HMUtilities.h"
#include "HMGridPropertiesComponent.h"
#include "HMGridNavigationComponent.h"
#include "HMActorNavigationComponent.h"
#include "Navigation/PathFollowingComponent.h"

AHexMapDemoPlayerController::AHexMapDemoPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void AHexMapDemoPlayerController::BeginPlay()
{
	Super::BeginPlay();

	AHMGrid* Grid = FHMUtilities::GetGrid(GetWorld());
	if (Grid)
	{
		UHMGridPropertiesComponent* GridPropertiesComponent = Grid->FindComponentByClass<UHMGridPropertiesComponent>();
		if (!GridPropertiesComponent)
		{
			UE_LOG(LogTemp, Error, TEXT("Can't find GridPropertiesComponent"));
		}
		else
		{
			GridPropertiesComponent->SetupBaseProperties();
			GridPropertiesComponent->SetupTilesProperties();
			GridPropertiesComponent->SetupTilesNeighbours();
		}

		UHMGridNavigationComponent* GridNavigationComponent = Grid->FindComponentByClass<UHMGridNavigationComponent>();
		if (!GridNavigationComponent)
		{
			UE_LOG(LogTemp, Error, TEXT("Can't find GridNavigationComponent"));
		}
		else
		{
			GridNavigationComponent->SetupNavigation();

			APawn* const Pawn = GetPawn();
			if (Pawn)
			{
				UHMActorNavigationComponent* NavigationComponent = Pawn->FindComponentByClass<UHMActorNavigationComponent>();
				if (NavigationComponent)
				{
					NavigationComponent->GridNavigationComponent = GridNavigationComponent;
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find Grid"));
	}
}

void AHexMapDemoPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}
}

void AHexMapDemoPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &AHexMapDemoPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &AHexMapDemoPlayerController::OnSetDestinationReleased);

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AHexMapDemoPlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AHexMapDemoPlayerController::MoveToTouchLocation);

	InputComponent->BindAction("ResetVR", IE_Pressed, this, &AHexMapDemoPlayerController::OnResetVR);
}

void AHexMapDemoPlayerController::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AHexMapDemoPlayerController::MoveToMouseCursor()
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		if (AHexMapDemoCharacter* MyPawn = Cast<AHexMapDemoCharacter>(GetPawn()))
		{
			if (MyPawn->GetCursorToWorld())
			{
				
			}
		}
	}
	else
	{
		// Trace to see what is under the mouse cursor
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);

		if (Hit.bBlockingHit)
		{
			// We hit something, move there
			SetNewMoveDestination(Hit.ImpactPoint);
		}
	}
}

void AHexMapDemoPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void AHexMapDemoPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const Pawn = GetPawn();
	if (Pawn)
	{
		//UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
		float const Distance = FVector::Dist(DestLocation, Pawn->GetActorLocation());
		UHMActorNavigationComponent* NavigationComponent = Pawn->FindComponentByClass<UHMActorNavigationComponent>();
		// We need to issue move command only if far enough in order for walk animation to play correctly
		if (NavigationComponent && (Distance > 120.0f))
		{
			UPathFollowingComponent* PathFollowingComponent = FindComponentByClass<UPathFollowingComponent>();
			if (PathFollowingComponent != nullptr)
			{
				PathFollowingComponent->SetPreciseReachThreshold(0.f, 0.f);
			}

			NavigationComponent->MoveToLocation(this, DestLocation);
		}
	}
}

void AHexMapDemoPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void AHexMapDemoPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}
