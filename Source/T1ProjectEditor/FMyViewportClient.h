#pragma once

#include "CoreMinimal.h"
#include "EditorViewportClient.h"

class UMyUI;

class FMyViewportClient : public FEditorViewportClient, public TSharedFromThis<FMyViewportClient>
{
public:
	// 생성자에서 모든 필요한 기본 변수를 설정.
	FMyViewportClient(TWeakPtr<class FMyEditor> ParentIGCEditor, const TSharedRef<class FAdvancedPreviewScene>& AdvPreviewScene, const TSharedRef<class SMyViewport>& Viewport, UMyUI* ObjectToEdit);
	~FMyViewportClient();

	// 에디터에서 사용하는 입력
	virtual void Tick(float DeltaSeconds) override;
	virtual void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI) override;


private:
	TWeakPtr<class FMyEditor> MyEditorPtr;
	TWeakPtr<class SMyViewport> MyEditorViewportPtr;
	UMyUI* MyObject;
	class FAdvancedPreviewScene* AdvancedPreviewScene;

};