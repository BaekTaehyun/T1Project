// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


DECLARE_LOG_CATEGORY_EXTERN(T1Project, Log, All);

// 커스터마이징된 로그 (나중 릴리즈 빌드에선 끄는 용도이다)
#define T1LOG_CALLINFO (FString(__FUNCTION__) + TEXT(" ----Line(") + FString::FromInt(__LINE__) + TEXT(")"))
#define T1LOG_S(Verbosity)	UE_LOG(T1Project,Verbosity, TEXT("----%s"), *T1LOG_CALLINFO)
#define T1LOG(Verbosity, Format, ...)	UE_LOG(T1Project, Verbosity, TEXT("----%s ----%s"), *T1LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))


// 붉은색 로깅
#define T1CHECK(Expr, ...) {if(!(Expr)) {T1LOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__;}}