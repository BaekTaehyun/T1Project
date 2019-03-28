#pragma once
#include "Engine/DataTable.h"
#include "EmblemFrame.generated.h"//This is Generate File, If Change Anything Lose that Later. by BSAM 

//------------------------------------------------------------------
// EmblemFrameUStruct
//------------------------------------------------------------------
USTRUCT(BlueprintType, meta = (ScriptName = "EmblemFrame", DisplayName = "EmblemFrame"))
struct FEmblemFrame : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:	// UPROPERTY
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EmblemFrame")
	FString param;
};
