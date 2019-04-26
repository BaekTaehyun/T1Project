// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


DECLARE_LOG_CATEGORY_EXTERN(GameService, Log, All);

// Ŀ���͸���¡�� �α� (���� ������ ���忡�� ���� �뵵�̴�)
#define GMLOG_CALLINFO (FString(__FUNCTION__) + TEXT(" ----Line(") + FString::FromInt(__LINE__) + TEXT(")"))
#define GMLOG_S(Verbosity)	UE_LOG(GameService,Verbosity, TEXT("----%s"), *T1LOG_CALLINFO)
#define GMLOG(Verbosity, Format, ...)	UE_LOG(GameService, Verbosity, TEXT("----%s ----%s"), *T1LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))


// ������ �α�
#define GMCHECK(Expr, ...) {if(!(Expr)) {T1LOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__;}}