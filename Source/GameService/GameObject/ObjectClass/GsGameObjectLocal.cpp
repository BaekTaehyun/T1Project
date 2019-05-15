#include "GsGameObjectLocal.h"
#include "GameObject/ActorExtend/GsLocalCharacter.h"
#include "GameObject/State/GsFSMManager.h"
#include "GameObject/Skill/GsSKillLocal.h"
#include "GameObject/Parts/GsPartsLocal.h"
#include "GameObject/State/GsStateLocal.h"
#include "GameObject/Movement/GsMovementLocal.h"
#include "GameObject/Input/GsInputBindingLocalPlayer.h"

#include "Runtime/Engine/Classes/GameFramework/Controller.h"
#include "Runtime/Engine/Classes/Components/InputComponent.h"

//프로퍼티
EGsGameObjectType	UGsGameObjectLocal::GetObjectType() const { return EGsGameObjectType::LocalPlayer; }
AActor*				UGsGameObjectLocal::GetActor() const	    { return GetLocal(); }
AGsLocalCharacter*	UGsGameObjectLocal::GetLocal() const     { return Actor; }
FGsFSMManager*		UGsGameObjectLocal::GetBaseFSM() const    { return Fsm; }
FGsFSMManager*		UGsGameObjectLocal::GetUpperFSM() const   { return UpperFsm; }
FGsSkillBase*		UGsGameObjectLocal::GetSkill() const      { return Skill; }
FGsPartsBase*		UGsGameObjectLocal::GetParts() const      { return Parts; }
//

void UGsGameObjectLocal::Initialize()
{
	Super::Initialize();

    //타입 정의
    SET_FLAG_TYPE(ObjectType, UGsGameObjectLocal::GetObjectType());
    
	Fsm = new FGsFSMManager();
	Fsm->Initialize(this);

	UpperFsm = new FGsFSMManager();
	UpperFsm->Initialize(this);

	Actor = NULL;
}

void UGsGameObjectLocal::DeInitialize()
{
	Super::DeInitialize();
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
        Skill->LoadData(TEXT("SkillDataContainerBase'/Game/Resource/DataAsset/LocalSkills.LocalSkills'"));

        Parts = new FGsPartsLocal();
        Parts->Initialize(this);
        //임시 데이터 적용
        Parts->LoadData(TEXT("PartsDataContainerBase'/Game/Resource/DataAsset/LocalParts.LocalParts'"));

        //기본 상태 설정
        Fsm->ChangeState<FGsStateSpawn>();

        //모든 파츠 장착
        Parts->AttachAll();
	}
}

void UGsGameObjectLocal::Update(float delta)
{
	Super::Update(delta);

	if (UpperFsm) { UpperFsm->Update(this, delta); }
}