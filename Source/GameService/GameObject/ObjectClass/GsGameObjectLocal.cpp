#include "GsGameObjectLocal.h"
#include "GameObject/ActorExtend/GsLocalCharacter.h"
#include "GameObject/State/GsFSMManager.h"
#include "GameObject/Skill/GsSKillLocal.h"
#include "GameObject/Parts/GsPartsLocal.h"
#include "GameObject/State/GsStateLocal.h"
#include "GameObject/Movement/GsMovementLocal.h"
#include "GameObject/Input/GsInputBindingLocalPlayer.h"
#include "GameObject/Event/GsGameObjectEventLocal.h"

#include "Runtime/Engine/Classes/GameFramework/Controller.h"
#include "Runtime/Engine/Classes/Components/InputComponent.h"

#include "../../Camera/GsCameraModeManager.h"
#include "EngineMinimal.h"

//프로퍼티
EGsGameObjectType	UGsGameObjectLocal::GetObjectType() const		{ return EGsGameObjectType::LocalPlayer; }
AActor*				UGsGameObjectLocal::GetActor() const			{ return GetLocalCharacter(); }
AGsLocalCharacter*	UGsGameObjectLocal::GetLocalCharacter() const	{ return (Actor->IsValidLowLevel()) ? Actor : NULL; }
FGsFSMManager*		UGsGameObjectLocal::GetBaseFSM() const			{ return Fsm; }
FGsFSMManager*		UGsGameObjectLocal::GetUpperFSM() const			{ return UpperFsm; }
FGsSkillBase*		UGsGameObjectLocal::GetSkill() const			{ return Skill; }
FGsPartsBase*		UGsGameObjectLocal::GetParts() const			{ return Parts; }
FGsGameObjectEventBase* UGsGameObjectLocal::GetEvent() const		{ return Event; }
//

void UGsGameObjectLocal::Initialize()
{
	Super::Initialize();

    //타입 정의
    SET_FLAG_TYPE(ObjectType, UGsGameObjectLocal::GetObjectType());

	Actor = NULL;
}

void UGsGameObjectLocal::Finalize()
{
	Super::Finalize();

	if (Event)
	{
		delete Event;
	}
}

void UGsGameObjectLocal::ActorSpawned(AActor* Spawn)
{
	Super::ActorSpawned(Spawn);

	if (Spawn)
	{
		//액터 저장
		Actor = Cast<AGsLocalCharacter>(Spawn);

		//키입력 바인딩
		Actor->GetInputBinder()->Initialize(this);

		Movement = new FGsMovementLocal();
		Movement->Initialize(this);

		Skill = new FGsSKillLocal();
		Skill->Initialize(this);
		//임시 데이터 적용
		Skill->LoadData(TEXT("GsSkillDataContainerBase'/Game/Resource/DataAsset/LocalSkill.LocalSkill'"));

        Parts = new FGsPartsLocal();
        Parts->Initialize(this);
        //임시 데이터 적용
        Parts->LoadData(TEXT("GsPartsDataContainerBase'/Game/Resource/DataAsset/LocalParts.LocalParts'"));

		Fsm = new FGsFSMManager();
		Fsm->Initialize<FGsStateLocalSpawn>(this);

		UpperFsm = new FGsFSMManager();
		UpperFsm->Initialize<FGsStateLocalUpperIdle>(this);

        //모든 파츠 장착
        Parts->AttachAll();

		// 캐릭터 세팅
		if (GsCameraModeSingle::Instance != nullptr)
		{
			GsCameraModeSingle::Instance->SetCharacter(this);
		}		
	}
}

void UGsGameObjectLocal::Update(float delta)
{
	Super::Update(delta);

	if (UpperFsm) { UpperFsm->Update(this, delta); }

	if (GsCameraModeSingle::Instance != nullptr)
	{
		GsCameraModeSingle::Instance->Update(delta);
	}
}

void UGsGameObjectLocal::RegistEvent()
{
	if (!Event)
	{
		Event = new FGsGameObjectEventLocal(this);
	}
	Event->RegistEvent();
}