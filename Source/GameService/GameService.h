// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


DECLARE_LOG_CATEGORY_EXTERN(GameService, Log, All);

// 커스터마이징된 로그 (나중 릴리즈 빌드에선 끄는 용도이다)
#define GSLOG_CALLINFO (FString(__FUNCTION__) + TEXT(" ----Line(") + FString::FromInt(__LINE__) + TEXT(")"))
#define GSLOG_S(Verbosity)	UE_LOG(GameService,Verbosity, TEXT("----%s"), *GSLOG_CALLINFO)
#define GSLOG(Verbosity, Format, ...)	UE_LOG(GameService, Verbosity, TEXT("----%s ----%s"), *GSLOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))


// 붉은색 로깅
#define GSCHECK(Expr, ...) {if(!(Expr)) {GSLOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__;}}

// quotation으로 감싸준다. 즉, 문자열로 만들어 줌
#define _QUOTE(X) #X
#define QUOTE(X) _QUOTE(X)

// pragma message 를 사용해서 컴파일 타임에 todo를 알려줌
#define todo(MSG) message(__FILE__ "(" QUOTE(__LINE__) ") : todo => " MSG) 