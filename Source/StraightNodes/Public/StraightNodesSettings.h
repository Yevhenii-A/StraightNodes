#pragma once
// Copyright 2026 Yevhenii Amelchenko | Silent Symmetry Team. All Rights Reserved.

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "StraightNodesSettings.generated.h"

UENUM()
enum class EStraightNodesStyle : uint8
{
    Manhattan UMETA(DisplayName = "Manhattan (90°)"),
    Subway    UMETA(DisplayName = "Subway (stub + 90°)"),
};

UCLASS(config = EditorPerProjectUserSettings, defaultconfig, meta = (DisplayName = "Straight Nodes"))
class STRAIGHTNODES_API UStraightNodesSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UStraightNodesSettings();

    UPROPERTY(config, EditAnywhere, Category = "Routing")
    EStraightNodesStyle RoutingStyle;

    UPROPERTY(config, EditAnywhere, Category = "Routing", meta = (EditCondition = "RoutingStyle == EStraightNodesStyle::Subway", ClampMin = "4.0", ClampMax = "120.0"))
    float SubwayStubLength;

    virtual FName GetCategoryName() const override { return TEXT("Plugins"); }
};
