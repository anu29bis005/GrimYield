#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GrimYieldGameInstance.generated.h"

UCLASS()
class SLASH_API UGrimYieldGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Difficulty")
	bool bIsHardMode = false;
};
