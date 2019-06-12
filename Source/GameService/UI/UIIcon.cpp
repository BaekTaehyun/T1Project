// Fill out your copyright notice in the Description page of Project Settings.


#include "UIIcon.h"
#include "Components/WidgetComponent.h"
#include "Overlay.h"

// ItemIcon �� �̹���, Name, ���� , Grade ��� Display �ϴ� ������Ʈ ����
UUIIcon::UUIIcon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UUIIcon::ConstructEventCall()
{
	
}

void UUIIcon::OnClickItemIconEvent()
{
	UE_LOG(LogTemp, Log, TEXT("OnClickItemIconEvent Call !!!"));
	//UOverlay* overlay;
	//overlay->AddChild(childWidget);

}

void UUIIcon::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (nullptr != Item_CountText)
	{
		FText testFText = FText::FromString("Change Value !!!");
		Item_CountText->SetText(testFText);
	}
}

void UUIIcon::SetItemImg()
{
	UE_LOG(LogTemp, Log, TEXT("Call SetItemImg !!"));

	// Texture Load 1
	//FString _path = "/Game/UI/Texture/icon_potion_01.icon_potion_01";
	//UTexture2D* tmpTexture = LoadTextureFromPath(_path);
	
	//Texture Load 2
	UTexture2D* tmpTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/Texture/icon_potion_01.icon_potion_01"),nullptr , LOAD_None , nullptr);
	if (nullptr == tmpTexture)
	{
		UE_LOG(LogTemp, Log, TEXT("UTexture2D LoadObj is Failed !!"));
		return;
	}
	else
	{
		ItemImg->SetBrushFromTexture(tmpTexture);
	}
}

UTexture2D* UUIIcon::LoadTextureFromPath(const FString& Path)
{
	if (Path.IsEmpty())
	{
		return nullptr;
	}

	return Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *(Path)));
}


void UUIIcon::TestLoadImgToTexture2D()
{
	/*
	const FString FullFilePath; //�ε��� ������ ����Դϴ�. 
	TArray<uint8> RawImageData; //�̹��� Raw�����͸� �� �迭�� �ε��մϴ�.
	UTexture2D* LoadedTexture2D = nullptr; //���������� �ε��� UTexture2D�� �� ������ �����մϴ�.

	//Raw������ �ε�. ������ true�� �����մϴ�.
	FFileHelper::LoadFileToArray(RawImageData, *FullFilePath);

	//ImageWrapper �ν��Ͻ� ����. UTexture2D�� �����ϱ����� Ŭ�����Դϴ�.
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	//�ε��� Raw�����͸� UTexture2D Ÿ������ ��ȯ�մϴ�.
	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawImageData.GetData(), RawImageData.Num()))
	{
		const TArray<uint8>* UncompressedBGRA = NULL;
		if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
		{
			//UTexture2D ����.
			LoadedTexture2D = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);

			if (LoadedTexture2D)
			{
				//�ȼ������͸� ó���ϴ� �κ��Դϴ�.
				void* TextureData = LoadedTexture2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, UncompressedBGRA->GetData(), UncompressedBGRA->Num());
				LoadedTexture2D->PlatformData->Mips[0].BulkData.Unlock();
				LoadedTexture2D->UpdateResource();
			}
		}
	}
	*/
}