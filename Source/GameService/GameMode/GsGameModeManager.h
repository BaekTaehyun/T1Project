#pragma once

#include "../Class/GsState.h"
#include "../Class/GsStateMng.h"
#include "../Class/GsSingleton.h"
#include "../Class/GsManager.h"
#include "GsGameMode.h"
#include "GsGameModeBase.h"

class FGsMessageNone;
//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �ν��Ͻ� �Ҵ������(���� �ν��Ͻ��� �ƴ� ��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class FGsGameModeAllocator : TGsMapAllocator<FGsGameMode::Mode, FGsGameModeBase>
{
public:
	FGsGameModeAllocator() {}
	virtual ~FGsGameModeAllocator() {}
	virtual FGsGameModeBase* Alloc(FGsGameMode::Mode inMode) override;
};

//------------------------------------------------------------------------------
// ���Ӹ�带 ����
//------------------------------------------------------------------------------
class FGsGameModeManager : 
	public TGsStateMng<FGsGameMode::Mode, FGsGameModeBase, FGsGameModeAllocator>,
	public TGsSingleton<FGsGameModeManager>,
	public IGsManager
{
	typedef TGsStateMng<FGsGameMode::Mode, FGsGameModeBase, FGsGameModeAllocator> Super;
public:
	virtual ~FGsGameModeManager();

	//IGsManager
	virtual void Initialize() override;
	virtual void Finalize() override;

	void OnReconnectionStart();
	void OnReconnectionEnd();
};

typedef TGsSingleton<FGsGameModeManager> FGsGameModeSingle;
#define GMode() FGsGameModeSingle::Instance