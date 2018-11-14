#pragma once

#include "CoreMinimal.h"
#include "EditorViewportClient.h"

class UMyUI;

class FMyViewportClient : public FEditorViewportClient, public TSharedFromThis<FMyViewportClient>
{
public:
	// �����ڿ��� ��� �ʿ��� �⺻ ������ ����.
	FMyViewportClient(TWeakPtr<class FMyEditor> ParentIGCEditor, const TSharedRef<class FAdvancedPreviewScene>& AdvPreviewScene, const TSharedRef<class SMyViewport>& Viewport, UMyUI* ObjectToEdit);
	~FMyViewportClient();

	// �����Ϳ��� ����ϴ� �Է�
	virtual void Tick(float DeltaSeconds) override;
	virtual void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI) override;


private:
	TWeakPtr<class FMyEditor> MyEditorPtr;
	TWeakPtr<class SMyViewport> MyEditorViewportPtr;
	UMyUI* MyObject;
	class FAdvancedPreviewScene* AdvancedPreviewScene;

};