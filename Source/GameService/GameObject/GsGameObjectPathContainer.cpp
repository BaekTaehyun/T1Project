// Fill out your copyright notice in the Description page of Project Settings.


#include "GsGameObjectPathContainer.h"

FString UGsGameObjectPathContainer::Get(EGameObjectResType Type)
{
	auto find = ListDataPath.FindByPredicate([=](const FGameObjectResPath& el)
		{
			return el.Type == Type;
		});

	if (find)
	{
		return find->Path.ToString();
	}

	return TEXT("");
}