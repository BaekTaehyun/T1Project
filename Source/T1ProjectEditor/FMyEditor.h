// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "Toolkits/IToolkitHost.h"

class FMyEditor : public FAssetEditorToolkit
{
public:
	//FMyEditor();
	~FMyEditor();

	// �ʱ�ȭ �Լ�. 
	void InitFMyEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, class UMyUI* InIGC);

	// IToolkit���� ��ӹ޾� �����ؾ� �� �����Լ���.
	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FString GetDocumentationLink() const override
	{
		return TEXT("NotAvailable");
	}

private:
	// Slate �� ������ �Լ�
	TSharedRef<SDockTab> SpawnTab_Viewport(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Detail(const FSpawnTabArgs& Args);

	TSharedRef<SDockTab> SpawnTab_PreviewSceneSettings(const FSpawnTabArgs& Args);

private:
	// My �����Ͱ� ����� ������ ���� ��Ī.
	static const FName MyEditorAppIdentifier;

	// �� Tab�� ����� ���� ��Ī.
	static const FName ViewportTabId;
	static const FName DetailTabId;
	static const FName PreviewSceneSettingsTabId;

	// ������ ��
	TSharedPtr<class IDetailsView> DetailsView;

	// ������ ����
	TSharedPtr<class SMyViewport> Viewport;

	// ������ My Obj
	class UMyUI* MyObj;

	// ������ �� ���� ����.
	TSharedPtr<SWidget> AdvancedPreviewSettingsWidget;
};
