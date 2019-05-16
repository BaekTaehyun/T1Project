// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsSkillBase.h"
#include "Process/GsSkillActionNodeBase.h"

class ALocalPlayerObject;
/**
 * 
 */
class GAMESERVICE_API FGsSKillLocal : public FGsSkillBase
{
public:
	FGsSKillLocal();
	virtual ~FGsSKillLocal();

	virtual void Initialize(UGsGameObjectBase* Owner) override;

	virtual void LoadData(const TCHAR * Path) override;
	virtual void UseSKill(int ID) override;

	virtual void OnSKillNode() override;
	virtual void RunSkillNode(float DeltaTime) override;

	TArray<FGsSkillActionNodeBase*>* GetSKillNodes(int ID);

private:
	void LoadSKillNode();
	FGsSkillActionNodeBase* CreateSkillNode(const FGsSkillActionDataBase& Data) const;

private:
	//스킬 액션 정보들
	TMap<int, TArray<FGsSkillActionNodeBase*>> MapSkillNodes;
	TArray<FGsSkillActionNodeBase*> RunSkillNodes;
	TArray<FGsSkillActionNodeBase*> UseSkillNodes;
};
