// Fill out your copyright notice in the Description page of Project Settings.


#include "GsUINonDestructiveWidget.h"


UGsUINonDestructiveWidget::UGsUINonDestructiveWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetEnableAutoDestroy(false);

}

void UGsUINonDestructiveWidget::RemoveFromParent()
{
	// ȭ�鿡�� �����ϴ� �� ����
	if (bEnableAutoDestroy)
	{
		Super::RemoveFromParent();
	}
}

void UGsUINonDestructiveWidget::SetEnableAutoDestroy(bool bInEnableAutoDestroy)
{
	bEnableAutoDestroy = bInEnableAutoDestroy;

	// GC����
	// RF_MarkAsRootSet : Object will be marked as root set on construction and not be garbage collected, 
	// even if unreferenced (DO NOT USE THIS FLAG in HasAnyFlags() etc)
	int32 CurrentFlags = (int32)(GetFlags());
	if (bEnableAutoDestroy)
	{
		SetFlags((EObjectFlags)(CurrentFlags | RF_MarkAsRootSet));
	}
	else
	{
		SetFlags((EObjectFlags)(CurrentFlags ^ RF_MarkAsRootSet));
	}
}
