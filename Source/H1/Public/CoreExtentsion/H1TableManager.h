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
	// DT�� �����ϴ� �ڵ尡 ó�� ����Ǵ� ���� ���̺� �ε带 �����Ѵ�.
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

		// TODO :  GetTableCS ���� �α� ǥ�ÿ� ������ ������� �ۼ��ص� �ڵ�� ������.
		// TODO : ���̺� Ű�� ID�� �и��Ȱ� ��ġ�� �� ã�� �� ��.
		// TableKey ItemID�� ���� �ɷ� �ϰ� ����. �Է½ÿ� ���� ������� ������ ����.
		static const FString GetTableCS(TEXT("UH1TableManager::GetTable"));
		T* RetTable = TargetTableAsset->FindRow<T>(TableKey, GetTableCS);

		return RetTable;
	}

public:
	UPROPERTY()
		TMap<FName, UDataTable*> DataTableMap;
};
