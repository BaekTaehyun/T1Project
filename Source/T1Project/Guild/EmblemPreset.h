#pragma once
#include "Engine/DataTable.h"
#include "EmblemPreset.generated.h"//This is Generate File, If Change Anything Lose that Later. by BSAM 

//------------------------------------------------------------------
// EmblemPresetUStruct
//------------------------------------------------------------------
USTRUCT(BlueprintType, meta = (ScriptName = "EmblemPreset", DisplayName = "EmblemPreset"))
struct FEmblemPreset : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:	// UPROPERTY
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EmblemPreset")
	int frameId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EmblemPreset")
	int symbolId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EmblemPreset")
	int colorId;
};
