// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "IDetailCustomization.h"
#include "DetailLayoutBuilder.h"
#include "ModuleManager.h"
#include "PropertyEditorModule.h"

class FMyDetailsCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
};