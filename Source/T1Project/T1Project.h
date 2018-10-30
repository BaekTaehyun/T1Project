// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


DECLARE_LOG_CATEGORY_EXTERN(T1Project, Log, All);

// Ŀ���͸���¡�� �α� (���� ������ ���忡�� ���� �뵵�̴�)
#define T1LOG_CALLINFO (FString(__FUNCTION__) + TEXT(" ----Line(") + FString::FromInt(__LINE__) + TEXT(")"))
#define T1LOG_S(Verbosity)	UE_LOG(T1Project,Verbosity, TEXT("----%s"), *T1LOG_CALLINFO)
#define T1LOG(Verbosity, Format, ...)	UE_LOG(T1Project, Verbosity, TEXT("----%s ----%s"), *T1LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))


// ������ �α�
#define T1CHECK(Expr, ...) {if(!(Expr)) {T1LOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__;}}