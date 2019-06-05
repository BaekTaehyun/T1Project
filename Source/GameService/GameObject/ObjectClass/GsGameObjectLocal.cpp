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

	if (InputBinder)
	{
		InputBinder = NULL;
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
		InputBinder = NewObject<UGsInputBindingLocalPlayer>();
		InputBinder->Initialize(this);
		Actor->SetInputBinder(InputBinder);

		//무브먼트 생성
		Movement = new FGsMovementLocal();
		Movement->Initialize(this);

		//스킬 핸들러 생성
		Skill = new FGsSKillLocal();
		Skill->Initialize(this);
		//임시 데이터 적용
		Skill->LoadData(TEXT("GsSkillDataContainerBase'/Game/Resource/DataAsset/LocalSkill.LocalSkill'"));

		//파츠 핸들러 생성
        Parts = new FGsPartsLocal();
        Parts->Initialize(this);
        //임시 데이터 적용
        Parts->LoadData(TEXT("GsPartsDataContainerBase'/Game/Resource/DataAsset/LocalParts.LocalParts'"));

		//FSM생성
		Fsm = new FGsFSMManager();
		Fsm->Initialize<FGsStateLocalSpawn>(this);

		//상체 FSM생성
		UpperFsm = new FGsFSMManager();
		UpperFsm->Initialize<FGsStateLocalUpperIdle>(this);

        //모든 파츠 장착
        Parts->AttachAll();

		// 캐릭터 세팅
		if (GCamera() != nullptr)
		{
			GCamera()->SetCharacter(this);
		}		
	}
}

void UGsGameObjectLocal::Update(float delta)
{
	Super::Update(delta);

	if (UpperFsm) { UpperFsm->Update(this, delta); }

	if (GCamera() != nullptr)
	{
		GCamera()->Update(delta);
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