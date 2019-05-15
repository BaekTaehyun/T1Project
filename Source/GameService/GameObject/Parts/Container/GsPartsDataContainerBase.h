#pragma once

#include "CoreMinimal.h"
#include "GameObject/Parts/Data/GsPartsDataBase.h"
#include "Runtime/Engine/Classes/Engine/DataAsset.h"
#include "GsPartsDataContainerBase.generated.h"


UCLASS()
class GAMESERVICE_API UGsPartsDataContainerBase : public UDataAsset
{
	GENERATED_BODY()

public:
	FORCEINLINE const TArray<FGsPartsDataBase>& GetPartsData() { return Parts; }

	UPROPERTY(EditAnywhere)
	FSkeletalMeshMergeParams Params;

	UPROPERTY(EditAnywhere)
	TArray<FGsPartsDataBase> Parts;
};
