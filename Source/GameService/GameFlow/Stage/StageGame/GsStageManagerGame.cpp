#include "GsStageManagerGame.h"


//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �ν��Ͻ� �Ҵ������(���� �ν��Ͻ��� �ƴ� ��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
FGsStageGameBase* FGsStageGameAllocator::Alloc(FGsStageMode::Game inMode)
{
	return NULL;
}
//------------------------------------------------------------------------------
// ���Ӹ�带 ����
//------------------------------------------------------------------------------
void FGsStageManagerGame::RemoveAll()
{
	Super::RemoveAll();
}
void FGsStageManagerGame::InitState()
{
	Super::InitState();
}
