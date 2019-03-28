#pragma once
#include "Engine/DataTable.h"
#include "EmblemColor.generated.h"//This is Generate File, If Change Anything Lose that Later. by BSAM 

//------------------------------------------------------------------
// EmblemColorUStruct
//------------------------------------------------------------------
USTRUCT(BlueprintType, meta = (ScriptName = "EmblemColor", DisplayName = "EmblemColor"))
struct FEmblemColor : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:	// UPROPERTY
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EmblemColor")
	FString param;
};
