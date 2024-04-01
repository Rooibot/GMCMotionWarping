// 


#include "GMCMotionWarpedMovementCmp.h"

#include "GMCMotionWarpingComponent.h"


// Sets default values for this component's properties
UGMCMotionWarpedMovementCmp::UGMCMotionWarpedMovementCmp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGMCMotionWarpedMovementCmp::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UGMCMotionWarpedMovementCmp::MontageUpdate(float DeltaSeconds)
{
  bHasRootMotion = false;
  
  if (!SkeletalMesh || !MontageTracker.HasActiveMontage())
  {
    MontageTracker.ClearActiveMontage();
    RootMotionParams.Clear();
    return;
  }

  if (MontageTracker.bMontagePaused)
  {
    return;
  }
  
  FGMC_AnimMontageInstance MontageInstance{MontageTracker.Montage, MontageTracker.MontagePosition, MontageTracker.MontagePlayRate, true};
  
  bool bFinishedBlendIn = false;
  bool bStartedBlendOut = false;
  
  TArray<const FAnimNotifyEvent*> MontageNotifyBeginEvents{};
  TArray<const FAnimNotifyEvent*> MontageNotifyEndEvents{};
  
  {
    gmc_ck(MontageTracker.HasActiveMontage())
    gmc_ck(!MontageTracker.bMontageEnded)
    gmc_ck(!MontageTracker.bMontagePaused)
  
    MontageTracker.bMontageEnded = MontageInstance.Advance(
      DeltaSeconds,
      RootMotionParams,
      bFinishedBlendIn,
      bStartedBlendOut,
      MontageNotifyBeginEvents,
      MontageNotifyEndEvents
    );
  
    MontageTracker.MontagePosition = MontageInstance.GetPosition();
  }
  
  gmc_ck(MontageTracker.MontagePosition >= 0.)
  gmc_ck(MontageTracker.MontagePosition <= MontageTracker.Montage->GetPlayLength())
  
  const FGMC_RootMotionExtractionSettings ExtractionSettings = GetRootMotionExtractionMetaData(MontageTracker.Montage);
  
  PreProcessRootMotion(MontageInstance, RootMotionParams, ExtractionSettings, DeltaSeconds);
  
  if (RootMotionParams.bHasRootMotion)
  {
    bHasRootMotion = true;
  
    // The root motion translation can be scaled by the user.
    RootMotionParams.ScaleRootMotionTranslation(GetAnimRootMotionTranslationScale());
  
    // Root motion calculations from motion warping
  	FTransform PreProcessedRootMotion;
  	if (ProcessRootMotionPreConvertToWorld.IsBound())
  	{
  		PreProcessedRootMotion = ProcessRootMotionPreConvertToWorld.Execute(RootMotionParams.GetRootMotionTransform(), this, DeltaSeconds);
  	}
  	else
  	{
  		PreProcessedRootMotion = RootMotionParams.GetRootMotionTransform();
  	}
    const FTransform WorldSpaceRootMotionTransform = SkeletalMesh->ConvertLocalRootMotionToWorld(PreProcessedRootMotion);
  
    // Save the root motion transform in world space.
    RootMotionParams.Set(WorldSpaceRootMotionTransform);
  
    // Calculate the root motion velocity from the world space root motion translation.
    CalculateAnimRootMotionVelocity(ExtractionSettings, DeltaSeconds);
  
    // Apply the root motion rotation now. Translation is applied with the next update from the calculated velocity. Splitting root motion translation and
    // rotation up like this may not be optimal but the alternative is to replicate the rotation for replay which is far more undesirable.
    ApplyAnimRootMotionRotation(ExtractionSettings, DeltaSeconds);
  }
  
  RootMotionParams.Clear();
  
  CallMontageEvents(MontageTracker, bFinishedBlendIn, bStartedBlendOut, MontageNotifyBeginEvents, MontageNotifyEndEvents, DeltaSeconds);
  
  gmc_ck(!MontageTracker.bStartedNewMontage)
  gmc_ck(!MontageTracker.bMontageEnded)
  gmc_ck(!MontageTracker.bInterruptedPreviousMontage)	
}

// Called every frame
void UGMCMotionWarpedMovementCmp::TickComponent(float DeltaTime, ELevelTick TickType,
                                                FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

