#pragma once


#include "../Class/GsContainer.h"
#include "../Class/GsSingleton.h"

#include "GsControlMode.h"
#include "GsCameraModeBase.h"

#define NEW_CAM_CHAR

//--------------------------------------------------------------
// 카메라 모드 할당관리자
//--------------------------------------------------------------
class GsCameraModeAllocator : TGsMapAllocator<EGsControlMode, GsCameraModeBase>
{
public:
	GsCameraModeAllocator() {}
	virtual ~GsCameraModeAllocator() {}
	virtual GsCameraModeBase* Alloc(EGsControlMode In_mode) override;
};


//--------------------------------------------------------------
// 카메라 모드 관리자
// 기존 TGsStateMng 의 구조에서 
// 인자 변환 enter, exit, update에서 ACharacter 넘기기위해서 별도로 만듬...
//--------------------------------------------------------------
struct FGsCamModeData;
class UGsGameObjectLocal;
class GAMESERVICE_API GsCameraModeManager :
	public GSTMap<EGsControlMode, GsCameraModeBase, GsCameraModeAllocator>,
	public TGsSingleton<GsCameraModeManager>
{
	

	TSharedPtr<GsCameraModeBase>	_currentState = NULL;

	DECLARE_EVENT(GSTStateMng, MainEvent)
	DECLARE_EVENT_OneParam(GSTStateMng, StateEvent, EGsControlMode)

	MainEvent				_onInit;
	MainEvent				_onRemoveAll;

	StateEvent				_onEnterState;
	StateEvent				_onLeaveState;


	// 내 캐릭터
	UGsGameObjectLocal* LocalPlayer;
	// 테이블 데이터
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UDataTable* CamModeData;

	// 카메라 자동 회전 커브 데이터	
	UCurveFloat* CamAutoRotCurveData;
	// 모드별 데이터
	TMap<EGsControlMode, FGsCamModeData*> _mapModeData;
protected:
	TSharedRef<GsCameraModeBase>	GetState()
	{
		return _currentState.ToSharedRef();
	}
public:
	GsCameraModeManager() : GSTMap<EGsControlMode, GsCameraModeBase, GsCameraModeAllocator>() {};
	virtual ~GsCameraModeManager();

	MainEvent& OnInit()  { return _onInit; }
	MainEvent& OnRemoveAll()  { return _onRemoveAll; }
	StateEvent& OnEnterState()  { return _onEnterState; }
	StateEvent& OnLeaveState()  { return _onLeaveState; }

	UCurveFloat* GetCurveData() { return CamAutoRotCurveData; }
	//------------------------------------------------------------------------------
	virtual void RemoveAll()
	{
		_onRemoveAll.Broadcast();

		if (_currentState.IsValid())
		{
			_currentState.Get()->Exit(LocalPlayer);
			_currentState = NULL;
		}

		Clear();
	};

	//------------------------------------------------------------------------------
	// 추가(어떤상태가 있는지 상속에서 정의해서 추가해준다.)
	virtual void InitState()
	{
		_onInit.Broadcast();
	}


	// 초기화(처음 한번만)
	virtual void Initialize();

	// 캐릭터 세팅(초기화와 시점이 다를수 있음...)
	void SetCharacter(UGsGameObjectLocal* In_char);

	// 상태 변경(각 스테이트한테 캐릭터를 인자로 넘기는 버전)
	void ChangeState(EGsControlMode In_state);

	// 갱신
	void Update(float In_deltaTime);
	// 다음 스텝으로 진행
	void NextStep();
	// 모드에 따른 카메라 데이터 가져오기
	FGsCamModeData* GetCamModeData(EGsControlMode In_mode);
};
typedef TGsSingleton<GsCameraModeManager> GsCameraModeSingle;