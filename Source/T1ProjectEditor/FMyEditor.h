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

	// 초기화 함수. 
	void InitFMyEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, class UMyUI* InIGC);

	// IToolkit에서 상속받아 구현해야 할 가상함수들.
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
	// Slate 를 생성할 함수
	TSharedRef<SDockTab> SpawnTab_Viewport(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Detail(const FSpawnTabArgs& Args);

	TSharedRef<SDockTab> SpawnTab_PreviewSceneSettings(const FSpawnTabArgs& Args);

private:
	// My 에디터가 사용할 고유한 앱의 명칭.
	static const FName MyEditorAppIdentifier;

	// 각 Tab이 사용할 고유 명칭.
	static const FName ViewportTabId;
	static const FName DetailTabId;
	static const FName PreviewSceneSettingsTabId;

	// 디테일 뷰
	TSharedPtr<class IDetailsView> DetailsView;

	// 프리뷰 위젯
	TSharedPtr<class SMyViewport> Viewport;

	// 편집할 My Obj
	class UMyUI* MyObj;

	// 프리뷰 씬 세팅 위젯.
	TSharedPtr<SWidget> AdvancedPreviewSettingsWidget;
};
