// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillCollisionComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "Classes/Components/MeshComponent.h"
#include "Classes/Materials/Material.h"
#include "GameObject/ActorExtend/GsNpcPawn.h"

// Sets default values for this component's properties
USkillCollisionComponent::USkillCollisionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
#if (WITH_EDITOR)
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;
	bTickInEditor = true;
#endif
	// ...
}

// Called every frame
void USkillCollisionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (GetWorld() && !GetWorld()->IsGameWorld())
	{
		DrawCollision();
	}
	//FVector dest = GetComponentLocation() + GetForwardVector() * 100.f;
	//DrawDebugDirectionalArrow(GetWorld(), GetComponentLocation(), dest, 20.f, FColor::Black);
}

void USkillCollisionComponent::Set(const FGsSkillNotifyCollisionData* Data)
{
	Type = Data->Type;
	switch (Type)
	{
	case ESkillCollisionType::Circle:
		SetCircle(Data);
		break;
	case ESkillCollisionType::Fan:
		SetFan(Data);
		break;
	case ESkillCollisionType::Concave:
		SetConcave(Data);
		break;
	}

	if (UseDebugLine)
	{
		DrawCollision();
	}
}

void USkillCollisionComponent::Set(FTransform Tm, const FGsSkillNotifyCollisionData* Data)
{
	SetWorldTransform(Tm);
	Set(Data);
}

void USkillCollisionComponent::SetCircle(const FGsSkillNotifyCollisionData* Data)
{
	Circle.Radius = Data->Radius;
}

void USkillCollisionComponent::SetFan(const FGsSkillNotifyCollisionData* Data)
{
	Fan.Radius = Data->Radius;
	Fan.DegAngle = Data->DegAngle;
}

void USkillCollisionComponent::SetConcave(const FGsSkillNotifyCollisionData* Data)
{
	Concave.ListLine = Data->ListPoint;
}

bool USkillCollisionComponent::InSideCheck(FVector Point)
{
	switch (Type)
	{
	case ESkillCollisionType::Circle:
		return InSideCircle(Point);
	case ESkillCollisionType::Fan:
		return InSideFan(Point);
	case ESkillCollisionType::Concave:
		return InSideConcave(Point);
	}

	return false;
}

bool USkillCollisionComponent::InSideConcave(FVector Point)
{
	return InSideConcave(FVector2D(Point.X, Point.Y));
}

bool USkillCollisionComponent::InSideConcave(FVector2D Point)
{
	int ptcount = Concave.ListLine.Num();
	if (ptcount < 3)
		return false;

	int inside = 0;
	auto worldPos = FVector2D(GetComponentLocation().X, GetComponentLocation().Y);
	auto newPos = Concave.ListLine[ptcount - 1] + worldPos;

	uint32 xold = newPos.X;
	uint32 yold = newPos.Y;

	uint32 xnew = 0;
	uint32 ynew = 0;
	uint32 x1 = 0;
	uint32 y1 = 0;
	uint32 x2 = 0;
	uint32 y2 = 0;

	for (int i = 0; i < ptcount; ++i)
	{
		newPos = Concave.ListLine[i] + worldPos;
		xnew = newPos.X;
		ynew = newPos.Y;
		if (xnew > xold) {
			x1 = xold;
			x2 = xnew;
			y1 = yold;
			y2 = ynew;
		}
		else {
			x1 = xnew;
			x2 = xold;
			y1 = ynew;
			y2 = yold;
		}
		if ((xnew < Point.X) == (Point.X <= xold)          /* edge "open" at one end */
			&& ((long)Point.Y - (long)y1)*(long)(x2 - x1)
			< ((long)y2 - (long)y1)*(long)(Point.X - x1)) {
			inside = !inside;
		}
		xold = xnew;
		yold = ynew;
	}

	return(inside);
}

bool USkillCollisionComponent::InSideCircle(FVector Point)
{
	return InSideCircle(FVector2D(Point.X, Point.Y));
}

bool USkillCollisionComponent::InSideCircle(FVector2D Point)
{
	auto worldPos = FVector2D(GetComponentLocation().X, GetComponentLocation().Y);
	auto dir = Point - worldPos;
	return dir.SizeSquared() <= Circle.Radius * Circle.Radius;
}

bool USkillCollisionComponent::InSideFan(FVector Point)
{
	return InSideFan(FVector2D(Point.X, Point.Y));
}

bool USkillCollisionComponent::InSideFan(FVector2D Point)
{
	auto worldPos = FVector2D(GetComponentLocation().X, GetComponentLocation().Y);
	auto dir = Point - worldPos;
	if (dir.SizeSquared() <= Fan.Radius * Fan.Radius)
	{
		float targetLen = dir.Size();

		auto fanDir = GetForwardVector();
		auto fanDir2D = FVector2D(fanDir.X, fanDir.Y);
		auto targetDir = dir.GetSafeNormal();

		//DrawDebugDirectionalArrow(GetWorld(), GetComponentLocation(), GetComponentLocation() + fanDir * targetLen, 20.f, FColor::Yellow);
		//DrawDebugDirectionalArrow(GetWorld(), GetComponentLocation(), GetComponentLocation() + FVector(targetDir, GetComponentLocation().GetSafeNormal().Z) * targetLen, 20.f, FColor::Blue);

		float dot = FVector2D::DotProduct(fanDir2D, targetDir);
		dot = FMath::Acos(dot);
		dot = FMath::RadiansToDegrees(dot);

		return dot <= Fan.DegAngle * 0.5f;
	}

	return false;
}

void USkillCollisionComponent::TestCollision(AActor* ActorClass)
{
	if (ActorClass)
	{
		auto pos = ActorClass->GetActorLocation();
		if (auto npcActor = Cast<AGsNpcPawn>(ActorClass))
		{
			bool inSide = false;
			switch (Type)
			{
			case ESkillCollisionType::Circle:
				inSide = InSideCircle(pos);
				break;
			case ESkillCollisionType::Fan:
				inSide = InSideFan(pos);
				break;
			case ESkillCollisionType::Concave:
				inSide = InSideConcave(pos);
				break;
			default:
				break;
			}

			if (true == inSide)
			{
				npcActor->SetColor(FLinearColor::Red);
			}
			else
			{
				npcActor->SetColor(FLinearColor::White);
			}

			DrawDebugSphere(GetWorld(), pos, 10.f, 100.f, FColor::White);
		}
	}
}

#if WITH_EDITOR
void USkillCollisionComponent::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	PrimaryComponentTick.bCanEverTick = UseDebugLine;
}

void USkillCollisionComponent::DrawCollision()
{
	switch (Type)
	{
	case ESkillCollisionType::Circle:
		DrawCircle();
		break;
	case ESkillCollisionType::Fan:
		DrawFan();
		break;
	case ESkillCollisionType::Concave:
		DrawConcave();
		break;
	}
}

void USkillCollisionComponent::DrawCircle()
{
	auto worldPos = GetComponentLocation();

	DrawDebugCircle(
		GetWorld(), worldPos, Circle.Radius, 50, FColor::Red,
		false, 2.f, 0, 0.f, FVector(1.f, 0.f, 0.f), FVector(0.f, 1.f, 0.f)
	);
}

void USkillCollisionComponent::DrawFan()
{
	auto worldPos = GetComponentLocation();
	auto dir = GetForwardVector();
	float halfAngle = Fan.DegAngle * 0.5f;

	auto hypotenuse1 = worldPos + dir.RotateAngleAxis(halfAngle, FVector::UpVector) * Fan.Radius;
	auto hypotenuse2 = worldPos + dir.RotateAngleAxis(-halfAngle, FVector::UpVector) * Fan.Radius;

	DrawDebugLine(GetWorld(), worldPos, hypotenuse1, FColor::Red, false, 2.f);
	DrawDebugLine(GetWorld(), worldPos, hypotenuse2, FColor::Red, false, 2.f);

	/*
	DrawDebugCircle(
		GetWorld(), worldPos, Fan.Radius, 50, FColor::White,
		false, -1.f, 0, 0.f, FVector(1.f, 0.f, 0.f), FVector(0.f, 1.f, 0.f)
	);*/
	float nomar = halfAngle / 100.f;
	for (int i = 0; i < 100; ++i)
	{
		float delta = halfAngle - nomar * i;
		auto tempFanl = worldPos + dir.RotateAngleAxis(-delta, FVector::UpVector) * Fan.Radius;
		auto tempFanr = worldPos + dir.RotateAngleAxis(delta, FVector::UpVector) * Fan.Radius;

		DrawDebugPoint(GetWorld(), tempFanl, 1.f, FColor::Red, false, 2.f);
		DrawDebugPoint(GetWorld(), tempFanr, 1.f, FColor::Red, false, 2.f);
	}
}

void USkillCollisionComponent::DrawConcave()
{	
	float zPos = GetComponentLocation().Z;
	auto startpos = GetComponentTransform().TransformPosition(FVector(Concave.ListLine[0], zPos));

	for (int i = 1; i < Concave.ListLine.Num(); ++i)
	{
		auto endpos = GetComponentTransform().TransformPosition(FVector(Concave.ListLine[i], zPos));
		DrawDebugLine(GetWorld(), startpos, endpos, i % 2 == 0 ? FColor::Red : FColor::Yellow, false, 2.f);
		startpos = endpos;
	}

	DrawDebugLine(
		GetWorld(), GetComponentTransform().TransformPosition(FVector(Concave.ListLine[0], zPos)),
		startpos, Concave.ListLine.Num() % 2 == 0 ? FColor::Red : FColor::Yellow, false, 2.f
	);
}
#endif


