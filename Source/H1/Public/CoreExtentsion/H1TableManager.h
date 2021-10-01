// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item/H1ItemTableDef.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "H1TableManager.generated.h"

/**
 * 
 */
UCLASS()
class UH1TableManager : public UObject
{
	GENERATED_BODY()
	
public:
	// DT에 접근하는 코드가 처음 실행되는 순간 테이블 로드를 진행한다.
	template<typename T>
	const T* GetTable(int TableKey)
	{
		return GetTable<T>(*FString::Printf(TEXT("%d"), TableKey));
	}

	template<typename T>
	const T* GetTable(FName TableKey)
	{
		UDataTable* TargetTableAsset = nullptr;
		FName TableAssetKey = T::GetTableAssetPath();

		if (DataTableMap.Find(TableAssetKey) == false || DataTableMap[TableAssetKey] == nullptr)
		{
			UDataTable* TableAsset = nullptr;
			FSoftObjectPath TableAssetLoader(*TableAssetKey.ToString());
			TableAsset = Cast<UDataTable>(TableAssetLoader.TryLoad());
			if (!IsValid(TableAsset))
			{
				return nullptr;
			}

			DataTableMap.Add(TableAssetKey, TableAsset);
			TargetTableAsset = TableAsset;
		}
		else
		{
			TargetTableAsset = DataTableMap[TableAssetKey];
		}

		// TODO :  GetTableCS 오류 로그 표시용 문구임 마음대로 작성해도 코드랑 무관함.
		// TODO : 테이블 키랑 ID랑 분리된거 합치는 거 찾아 볼 것.
		// TableKey ItemID랑 같은 걸로 하고 있음. 입력시에 같게 맞춰줘야 오류가 없음.
		static const FString GetTableCS(TEXT("UH1TableManager::GetTable"));
		T* RetTable = TargetTableAsset->FindRow<T>(TableKey, GetTableCS);

		return RetTable;
	}

public:
	UPROPERTY()
		TMap<FName, UDataTable*> DataTableMap;
};
