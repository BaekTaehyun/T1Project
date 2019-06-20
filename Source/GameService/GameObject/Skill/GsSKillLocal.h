// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsSkillBase.h"
#include "Process/GsSkillNotifyNodeBase.h"

/**
 * 
 */
class GAMESERVICE_API FGsSKillLocal : public FGsSkillBase
{
	typedef FGsSkillBase Super;

private:
	//스킬 액션 정보들
	TMap<int, TArray<FGsSkillNotifyNodeBase*>> MapSkillNodes;
	TArray<FGsSkillNotifyNodeBase*> ListRunSkillNodes;
	TArray<FGsSkillNotifyNodeBase*> ListUseSkillNodes;


public:
	FGsSKillLocal();
	virtual ~FGsSKillLocal();

	virtual void Initialize(UGsGameObjectBase* Owner) override;
	virtual void Finalize() override;

public:
	virtual void UseSKill(int ID) override;

	virtual void OnSKillNode() override;
	virtual void RunSkillNode(float DeltaTime) override;

private:
	void LoadSKillNode();
	FGsSkillNotifyNodeBase* CreateSkillNode(const FGsSkillNotifyDataBase& Data) const;

public:
	TArray<FGsSkillNotifyNodeBase*>* GetSKillNodes(int ID);
};
