#pragma once
#include "Engine/DataTable.h"
#include "GuildBuff.generated.h"//This is Generate File, If Change Anything Lose that Later. by BSAM 

//------------------------------------------------------------------
// GuildBuffUStruct
//------------------------------------------------------------------
USTRUCT(BlueprintType, meta = (ScriptName = "GuildBuff", DisplayName = "GuildBuff"))
struct FGuildBuff : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:	// UPROPERTY
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildBuff")
	int guildBuffLevel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildBuff")
	int purchaseCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildBuff")
	int unlockLevel;
};
