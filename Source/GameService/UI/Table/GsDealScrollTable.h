// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "GsDealScrollTable.generated.h"

/**
 * 
 */
UCLASS()
class GAMESERVICE_API UGsDealScrollTable : public UObject
{
	GENERATED_BODY()
	
};

// �ʿ� enum ����
UENUM(BluePrintType)
enum class EGsDealScrollType : uint8
{
	GS_DEALSCROLL_HIT UMETA(DisplayName = "Hit"), // ������
	GS_DEALSCROLL_DAMAGE UMETA(DisplayName = "Damage"), // �¾���
	GS_DEALSCROLL_HEAL UMETA(DisplayName = "Heal"), // �ǰ� ��
};

USTRUCT(BlueprintType)
struct FGsTextBoxSetting
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextBoxSetting")
	EGsDealScrollType DealScrollType = EGsDealScrollType::GS_DEALSCROLL_HIT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextBoxSetting")
	int32 FontSize = 24;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextBoxSetting")
	FName TypefaceFontName = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextBoxSetting")
	FLinearColor Color = FLinearColor::White; //FSlateColor
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextBoxSetting")
	int32 OutlineSize = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextBoxSetting")
	FLinearColor OutlineColor = FLinearColor::Black;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextBoxSetting")
	FVector2D ShadowOffset = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextBoxSetting")
	FLinearColor ShadowColorAndOpacity = FLinearColor::Black;

public:
	FGsTextBoxSetting()
		//: FontSize(24), Color(FColor)
	{
	}
};


USTRUCT(BlueprintType)
struct FGsTableDealScroll : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	// Target and transformation info.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DealScroll")
	FVector RelativeLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DealScroll")
	FRotator RelativeRotation = FRotator::ZeroRotator; // ��... �� �� ����

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DealScroll")
	float LocationRandomOffset = 0.0f;

	//float RotationRandomOffset; // �ʿ� ��

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DealScroll")
	FName SocketName = TEXT(""); // �ʿ�� ����. ������ �׳� ������

	// Animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DealScroll")
	FName AnimationName = TEXT("");

	// ���ĸ� �ְ� ������ �� �������� �гο� �ֵ��� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DealScroll")
	FGsTextBoxSetting MainTextSetting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DealScroll")
	FGsTextBoxSetting SubTextSetting;

public:
	FGsTableDealScroll()
		//: WidgetClass(nullptr), bNotDestroy(false)
	{
	}
};