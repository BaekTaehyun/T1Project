#pragma once
#include "Engine/DataTable.h"
#include "GuildBuffBase.generated.h"//This is Generate File, If Change Anything Lose that Later. by BSAM 

//------------------------------------------------------------------
// GuildBuffBaseUStruct
//------------------------------------------------------------------
USTRUCT(BlueprintType, meta = (ScriptName = "GuildBuffBase", DisplayName = "GuildBuffBase"))
struct FGuildBuffBase : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:	// UPROPERTY
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildBuffBase")
	int guildBuffId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildBuffBase")
	FString guildBuffIcon;
};
