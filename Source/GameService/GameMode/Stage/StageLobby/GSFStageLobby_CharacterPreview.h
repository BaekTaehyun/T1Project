#pragma once
#include "GSFStageLobbyBase.h"

//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class GSFStageLobby_CharecterPreview: public GSFStageLobbyBase
{
public:
	GSFStageLobby_CharecterPreview();
	virtual ~GSFStageLobby_CharecterPreview();
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;
};

