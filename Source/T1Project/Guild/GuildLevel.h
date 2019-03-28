#pragma once
#include "Engine/DataTable.h"
#include "GuildLevel.generated.h"//This is Generate File, If Change Anything Lose that Later. by BSAM 

//------------------------------------------------------------------
// GuildLevelUStruct
//------------------------------------------------------------------
USTRUCT(BlueprintType, meta = (ScriptName = "GuildLevel", DisplayName = "GuildLevel"))
struct FGuildLevel : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:	// UPROPERTY
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildLevel")
	int requiredAttendence;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildLevel")
	int totalCapacity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildLevel")
	int sergeantCapacity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildLevel")
	int captainCapacity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildLevel")
	int colonelCapacity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildLevel")
	int leaderCapacity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildLevel")
	TArray<int> GuildContentsOpen;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildLevel")
	TArray<int> goodsId;
};
