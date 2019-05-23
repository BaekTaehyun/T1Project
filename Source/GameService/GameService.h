// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


DECLARE_LOG_CATEGORY_EXTERN(GameService, Log, All);

// Ŀ���͸���¡�� �α� (���� ������ ���忡�� ���� �뵵�̴�)
#define GSLOG_CALLINFO (FString(__FUNCTION__) + TEXT(" ----Line(") + FString::FromInt(__LINE__) + TEXT(")"))
#define GSLOG_S(Verbosity)	UE_LOG(GameService,Verbosity, TEXT("----%s"), *GSLOG_CALLINFO)
#define GSLOG(Verbosity, Format, ...)	UE_LOG(GameService, Verbosity, TEXT("----%s ----%s"), *GSLOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))


// ������ �α�
#define GSCHECK(Expr, ...) {if(!(Expr)) {GSLOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__;}}

// quotation���� �����ش�. ��, ���ڿ��� ����� ��
#define _QUOTE(X) #X
#define QUOTE(X) _QUOTE(X)

// pragma message �� ����ؼ� ������ Ÿ�ӿ� todo�� �˷���
#define todo(MSG) message(__FILE__ "(" QUOTE(__LINE__) ") : todo => " MSG) 