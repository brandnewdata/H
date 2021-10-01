// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreExtentsion/H1_Singleton.h"


UH1_Singleton* UH1_Singleton::Get()
{
	// TODO : �𸮾󿡼� �̱��� �����ϱ� ����
	UH1_Singleton* SingletonObject = Cast<UH1_Singleton>(GEngine->GameSingleton);
	if (IsValid(SingletonObject))
	{
		return SingletonObject;
	}
	else
	{
		return NewObject<UH1_Singleton>(nullptr, UH1_Singleton::StaticClass());
	}
}