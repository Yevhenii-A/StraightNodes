#pragma once
// Copyright 2026 Yevhenii Amelchenko | Silent Symmetry Team. All Rights Reserved.

#include "CoreMinimal.h"
#include "EdGraphUtilities.h"
#include "BlueprintConnectionDrawingPolicy.h"
#include "Rendering/DrawElements.h"

class STRAIGHTNODES_API FStraightNodesConnectionDrawingPolicy : public FKismetConnectionDrawingPolicy
{
public:
	FStraightNodesConnectionDrawingPolicy(
		int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor,
		const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj);

	virtual void DrawSplineWithArrow(const FGeometry& StartGeom, const FGeometry& EndGeom, const FConnectionParams& Params) override;
	virtual void DrawSplineWithArrow(const FVector2f& StartPoint, const FVector2f& EndPoint, const FConnectionParams& Params) override;
	virtual void DrawPreviewConnector(const FGeometry& PinGeometry, const FVector2f& StartPoint, const FVector2f& EndPoint, UEdGraphPin* Pin) override;

private:
	void BuildManhattanPoints(const FVector2f& Start, const FVector2f& End, TArray<FVector2f>& OutPoints) const;
	void BuildSubwayPoints   (const FVector2f& Start, const FVector2f& End, TArray<FVector2f>& OutPoints) const;
	void BuildRoutePoints    (const FVector2f& Start, const FVector2f& End, TArray<FVector2f>& OutPoints) const;

	// Рисует полилайн и самостоятельно проверяет:
	//   - пересечение с SliceLine → ConnectionsIntersectingSliceLine
	//   - близость к мыши       → SplineOverlapResult
	void DrawPolylineAndTest(const TArray<FVector2f>& Points, const FConnectionParams& Params);

	// Проверка пересечения двух отрезков
	static bool SegmentsIntersect(const FVector2f& A1, const FVector2f& A2,
	                               const FVector2f& B1, const FVector2f& B2);
};

class STRAIGHTNODES_API FStraightNodesConnectionDrawingPolicyFactory : public FGraphPanelPinConnectionFactory
{
public:
	virtual FConnectionDrawingPolicy* CreateConnectionPolicy(
		const UEdGraphSchema* Schema, int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor,
		const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const override;
};
