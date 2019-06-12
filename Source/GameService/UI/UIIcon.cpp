// Fill out your copyright notice in the Description page of Project Settings.


#include "UIIcon.h"
#include "Components/WidgetComponent.h"
#include "Overlay.h"

// ItemIcon 에 이미지, Name, 수량 , Grade 등등 Display 하는 오브젝트 관리
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
	const FString FullFilePath; //로드할 파일의 경로입니다. 
	TArray<uint8> RawImageData; //이미지 Raw데이터를 이 배열에 로드합니다.
	UTexture2D* LoadedTexture2D = nullptr; //최종적으로 로드한 UTexture2D를 이 변수에 저장합니다.

	//Raw데이터 로드. 성공시 true를 리턴합니다.
	FFileHelper::LoadFileToArray(RawImageData, *FullFilePath);

	//ImageWrapper 인스턴스 생성. UTexture2D를 생성하기위한 클래스입니다.
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	//로드한 Raw데이터를 UTexture2D 타입으로 변환합니다.
	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawImageData.GetData(), RawImageData.Num()))
	{
		const TArray<uint8>* UncompressedBGRA = NULL;
		if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
		{
			//UTexture2D 생성.
			LoadedTexture2D = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);

			if (LoadedTexture2D)
			{
				//픽셀데이터를 처리하는 부분입니다.
				void* TextureData = LoadedTexture2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, UncompressedBGRA->GetData(), UncompressedBGRA->Num());
				LoadedTexture2D->PlatformData->Mips[0].BulkData.Unlock();
				LoadedTexture2D->UpdateResource();
			}
		}
	}
	*/
}