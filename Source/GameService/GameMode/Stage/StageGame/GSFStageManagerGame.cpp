#include "GSFStageManagerGame.h"


//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �ν��Ͻ� �Ҵ������(���� �ν��Ͻ��� �ƴ� ��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
GSFStageGameBase* GSFStageGameAllocator::Alloc(GSFStageMode::Game inMode)
{
	return NULL;
}
//------------------------------------------------------------------------------
// ���Ӹ�带 ����
//------------------------------------------------------------------------------
void GSStageManagerGame::RemoveAll()
{
	Super::RemoveAll();
}
void GSStageManagerGame::InitState()
{
	Super::InitState();
}
