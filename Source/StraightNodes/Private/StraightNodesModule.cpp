// Copyright 2026 Yevhenii Amelchenko | Silent Symmetry Team. All Rights Reserved.

#include "StraightNodesModule.h"
#include "StraightNodesConnectionDrawingPolicy.h"

#include "EdGraphUtilities.h"

static TSharedPtr<FStraightNodesConnectionDrawingPolicyFactory> GConnectionFactory;

void FStraightNodesModule::StartupModule()
{
    // Регистрируем политику отрисовки проводов
    GConnectionFactory = MakeShareable(new FStraightNodesConnectionDrawingPolicyFactory());
    FEdGraphUtilities::RegisterVisualPinConnectionFactory(GConnectionFactory);
}

void FStraightNodesModule::ShutdownModule()
{
    if (GConnectionFactory.IsValid())
    {
        FEdGraphUtilities::UnregisterVisualPinConnectionFactory(GConnectionFactory);
        GConnectionFactory.Reset();
    }
}

IMPLEMENT_MODULE(FStraightNodesModule, StraightNodes)
