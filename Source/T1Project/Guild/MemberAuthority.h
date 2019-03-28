#pragma once
#include "Engine/DataTable.h"
#include "MemberAuthority.generated.h"//This is Generate File, If Change Anything Lose that Later. by BSAM 

//------------------------------------------------------------------
// MemberAuthorityUStruct
//------------------------------------------------------------------
USTRUCT(BlueprintType, meta = (ScriptName = "MemberAuthority", DisplayName = "MemberAuthority"))
struct FMemberAuthority : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:	// UPROPERTY
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MemberAuthority")
	bool applicantList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MemberAuthority")
	bool approval;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MemberAuthority")
	bool refusal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MemberAuthority")
	bool guildAdmin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MemberAuthority")
	bool joinType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MemberAuthority")
	bool leaderChange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MemberAuthority")
	bool noticeEdit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MemberAuthority")
	bool introEdit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MemberAuthority")
	bool emblemChange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MemberAuthority")
	bool publicity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MemberAuthority")
	bool guildWithdrawal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MemberAuthority")
	bool exile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MemberAuthority")
	bool typeChange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MemberAuthority")
	bool buffLevelUp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MemberAuthority")
	bool levelup;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MemberAuthority")
	bool commandChatting;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MemberAuthority")
	bool executiveChatting;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MemberAuthority")
	int textId;
};
