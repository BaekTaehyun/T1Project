#pragma once
#include "GsStageLobbyBase.h"

//------------------------------------------------------------------------------
// �������� �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class FGsStageLobby_AssetDownLoad : public FGsStageLobbyBase
{
public:
	FGsStageLobby_AssetDownLoad();
	virtual ~FGsStageLobby_AssetDownLoad();
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;
};

