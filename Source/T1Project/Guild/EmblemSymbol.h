#pragma once
#include "Engine/DataTable.h"
#include "EmblemSymbol.generated.h"//This is Generate File, If Change Anything Lose that Later. by BSAM 

//------------------------------------------------------------------
// EmblemSymbolUStruct
//------------------------------------------------------------------
USTRUCT(BlueprintType, meta = (ScriptName = "EmblemSymbol", DisplayName = "EmblemSymbol"))
struct FEmblemSymbol : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:	// UPROPERTY
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EmblemSymbol")
	FString param;
};
