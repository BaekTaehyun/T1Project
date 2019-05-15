#pragma once
#include "GsStageLobbyBase.h"

//------------------------------------------------------------------------------
// 게임모드를 관리할때 필요한 메모리 할당관리자(상속구조도 지원하기 위함)
//------------------------------------------------------------------------------
class FGsStageLobby_CharecterPreview: public FGsStageLobbyBase
{
public:
	FGsStageLobby_CharecterPreview();
	virtual ~FGsStageLobby_CharecterPreview();
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;
};

