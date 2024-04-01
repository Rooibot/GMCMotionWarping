// 

#pragma once

#include "CoreMinimal.h"
#include "GMCOrganicMovementComponent.h"
#include "GMCRootMotionModifier.h"

#include "GMCMotionWarpedMovementCmp.generated.h"

DECLARE_DELEGATE_RetVal_ThreeParams(FTransform, FOnProcessRootMotion, const FTransform&, UGMC_OrganicMovementCmp*, float)


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GMCMOTIONWARPING_API UGMCMotionWarpedMovementCmp : public UGMC_OrganicMovementCmp
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGMCMotionWarpedMovementCmp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void MontageUpdate(float DeltaSeconds) override;
	
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	FOnProcessRootMotion ProcessRootMotionPreConvertToWorld;

};
