#pragma once

#include "EngineMinimal.h"
#include "SlateFwd.h"
#include "UObject/GCObject.h"
#include "SEditorViewport.h"
#include "SCommonEditorViewportToolbarBase.h"

class FMyEditor;
class UMyUI;

class SMyViewport : public SEditorViewport, public FGCObject
{
public:
	SLATE_BEGIN_ARGS(SMyViewport) {}
		SLATE_ARGUMENT(TWeakPtr<FMyEditor>, ParentMyEditor)
		SLATE_ARGUMENT(UMyUI*, ObjectToEdit)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	SMyViewport();
	~SMyViewport();

	// FGCObject �������̽�. U������Ʈ�� �ƴ϶� �������÷��� ȸ�� ����.
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	TSharedRef<class FAdvancedPreviewScene> GetPreviewScene();

protected:
	// �Ʒ� �Լ��� ������ ����Ʈ Ŭ���̾�Ʈ�� �������־�� �Ѵ�.
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;

private:
	// �θ� ������, ������ ��, ����Ʈ Ŭ���̾�Ʈ, �Ѱܹ��� IGC ��ü. 
	TWeakPtr<FMyEditor> MyEditorPtr;
	TSharedPtr<class FAdvancedPreviewScene> PreviewScene;
	TSharedPtr<class FMyViewportClient> MyViewportClient;
	UMyUI* MyObject;

	// �������̿� ����� ��Ƽ�� �ڽ� ����.
	TSharedPtr<SVerticalBox> OverlayTextVerticalBox;

	// �����並 ���� ����ƽ �޽� ������Ʈ.
	class UStaticMeshComponent* PreviewMeshComponent;
};