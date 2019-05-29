// Fill out your copyright notice in the Description page of Project Settings.


#include "GsGameObjectEventBase.h"

FGsGameObjectEventBase::FGsGameObjectEventBase(UGsGameObjectBase* owner)
{
	Owner = owner;
}

FGsGameObjectEventBase::~FGsGameObjectEventBase()
{
}

void FGsGameObjectEventBase::RegistEvent()
{
}

GsGameObjectEventParamBase* FGsGameObjectEventBase::GetParam(MessageGameObject::Action Type)
{
	auto findParam = EventParams.Find(Type);
	if (NULL == findParam)
	{
		findParam = &EventParams.Emplace(Type, CreateParam(Type));
	}
	return (*findParam);
}

void FGsGameObjectEventBase::ClearParam(MessageGameObject::Action Type)
{
	if (auto findParam = EventParams.Find(Type))
	{
		(*findParam)->Clear();
	}
}

GsGameObjectEventParamBase* FGsGameObjectEventBase::CreateParam(MessageGameObject::Action Type)
{
	return NULL;
}