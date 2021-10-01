// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EngineMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogH1, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(H1_Custom, Log, All);

// �����Ϸ��� ���� ������ �ִ� ��ũ�� Ȱ��...
#define H1LOG_CALLINFO (FString(__FUNCTION__) + TEXT("(")+FString::FromInt(__LINE__) + TEXT(")"))
#define H1LOG_S(Verbosity) UE_LOG(H1_Custom, Verbosity, TEXT("%s"), *H1LOG_CALLINFO)
#define H1LOG(Verbosity, Format, ...) UE_LOG(H1_Custom, Verbosity, TEXT("%s : %s"), *H1LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))