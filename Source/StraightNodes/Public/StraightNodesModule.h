#pragma once
// Copyright 2026 Yevhenii Amelchenko | Silent Symmetry Team. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FStraightNodesModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
