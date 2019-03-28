#pragma once
#include "Engine/DataTable.h"
#include "GuildBalance.generated.h"//This is Generate File, If Change Anything Lose that Later. by BSAM 

//------------------------------------------------------------------
// GuildBalanceUStruct
//------------------------------------------------------------------
USTRUCT(BlueprintType, meta = (ScriptName = "GuildBalance", DisplayName = "GuildBalance"))
struct FGuildBalance : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:	// UPROPERTY
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildBalance")
	int param;
};
