#pragma once


#include "../Class/GsContainer.h"
#include "../Class/GsSingleton.h"

#include "GsControlMode.h"
#include "GsCameraModeBase.h"



//--------------------------------------------------------------
// ī�޶� ��� �Ҵ������
//--------------------------------------------------------------
class GsCameraModeAllocator : TGsMapAllocator<EGsControlMode, GsCameraModeBase>
{
public:
	GsCameraModeAllocator() {}
	virtual ~GsCameraModeAllocator() {}
	virtual GsCameraModeBase* Alloc(EGsControlMode In_mode) override;
};


//--------------------------------------------------------------
// ī�޶� ��� ������
// ���� TGsStateMng �� �������� 
// ���� ��ȯ enter, exit, update���� ACharacter �ѱ�����ؼ� ������ ����...
//--------------------------------------------------------------
struct FGsCamModeData;
class GsCameraModeManager :
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


	// ��Ʈ�� �� ĳ����
	ACharacter* _character;

	// ���̺� ������
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UDataTable* CamModeData;

	// ��庰 ������
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

	ACharacter* GetCharacter(){ return _character; }
	//------------------------------------------------------------------------------
	virtual void RemoveAll()
	{
		_onRemoveAll.Broadcast();

		if (_currentState.IsValid())
		{
			_currentState.Get()->Exit(_character);
			_currentState = NULL;
		}

		Clear();
	};

	//------------------------------------------------------------------------------
	// �߰�(����°� �ִ��� ��ӿ��� �����ؼ� �߰����ش�.)
	virtual void InitState()
	{
		_onInit.Broadcast();
	}


	// �ʱ�ȭ(ó�� �ѹ���)
	virtual void Initialize();

	// ĳ���� ����(�ʱ�ȭ�� ������ �ٸ��� ����...)
	void SetCharacter(ACharacter* In_char);

	// ���� ����(�� ������Ʈ���� ĳ���͸� ���ڷ� �ѱ�� ����)
	void ChangeState(EGsControlMode In_state);

	// ����
	void Update(float In_deltaTime);
	// ���� �������� ����
	void NextStep();
	// ��忡 ���� ī�޶� ������ ��������
	FGsCamModeData* GetCamModeData(EGsControlMode In_mode);
};
typedef TGsSingleton<GsCameraModeManager> GsCameraModeSingle;