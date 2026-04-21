// Copyright 2026 Yevhenii Amelchenko | Silent Symmetry Team. All Rights Reserved.

#include "StraightNodesConnectionDrawingPolicy.h"
#include "StraightNodesSettings.h"

#include "EdGraphSchema_K2.h"
#include "EdGraph/EdGraph.h"
#include "Layout/Geometry.h"
#include "Rendering/DrawElements.h"

static const UStraightNodesSettings* GetSettings()
{
	return GetDefault<UStraightNodesSettings>();
}

FStraightNodesConnectionDrawingPolicy::FStraightNodesConnectionDrawingPolicy(
	int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor,
	const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj)
	: FKismetConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements, InGraphObj)
{
}

// ─────────────────────────────────────────────────────────────────────────────
//  Маршрутизация
// ─────────────────────────────────────────────────────────────────────────────

void FStraightNodesConnectionDrawingPolicy::BuildManhattanPoints(
	const FVector2f& Start, const FVector2f& End, TArray<FVector2f>& OutPoints) const
{
	OutPoints.Reset(4);
	OutPoints.Add(Start);

	const float DX = FMath::Abs(End.X - Start.X);
	const float DY = FMath::Abs(End.Y - Start.Y);

	if (DY < 2.f || DX < 2.f)
	{
		OutPoints.Add(End);
		return;
	}

	const float MidX = (Start.X + End.X) * 0.5f;
	OutPoints.Add(FVector2f(MidX, Start.Y));
	OutPoints.Add(FVector2f(MidX, End.Y));
	OutPoints.Add(End);
}

void FStraightNodesConnectionDrawingPolicy::BuildSubwayPoints(
	const FVector2f& Start, const FVector2f& End, TArray<FVector2f>& OutPoints) const
{
	const float StubLen = GetSettings()->SubwayStubLength * ZoomFactor;
	OutPoints.Reset(6);
	OutPoints.Add(Start);

	if (FMath::Abs(End.Y - Start.Y) < 2.f)
	{
		OutPoints.Add(End);
		return;
	}

	const float StubEndX   = Start.X + StubLen;
	const float StubStartX = End.X   - StubLen;

	if (StubEndX < StubStartX)
	{
		OutPoints.Add(FVector2f(StubEndX,   Start.Y));
		OutPoints.Add(FVector2f(StubEndX,   End.Y));
		OutPoints.Add(FVector2f(StubStartX, End.Y));
		OutPoints.Add(End);
	}
	else
	{
		BuildManhattanPoints(Start, End, OutPoints);
	}
}

void FStraightNodesConnectionDrawingPolicy::BuildRoutePoints(
	const FVector2f& Start, const FVector2f& End, TArray<FVector2f>& OutPoints) const
{
	if (GetSettings()->RoutingStyle == EStraightNodesStyle::Subway)
		BuildSubwayPoints(Start, End, OutPoints);
	else
		BuildManhattanPoints(Start, End, OutPoints);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Пересечение двух отрезков (2D)
// ─────────────────────────────────────────────────────────────────────────────

bool FStraightNodesConnectionDrawingPolicy::SegmentsIntersect(
	const FVector2f& A1, const FVector2f& A2,
	const FVector2f& B1, const FVector2f& B2)
{
	FVector Intersection;
	return FMath::SegmentIntersection2D(
		FVector(A1.X, A1.Y, 0), FVector(A2.X, A2.Y, 0),
		FVector(B1.X, B1.Y, 0), FVector(B2.X, B2.Y, 0),
		Intersection);
}

// ─────────────────────────────────────────────────────────────────────────────
//  DrawPolylineAndTest
//
//  Рисует полилайн напрямую через MakeLines и сам проверяет:
//    1. Пересечение каждого отрезка с SliceLine → разрез Alt+ЛКМ
//    2. Расстояние от мыши до каждого отрезка  → hover/tooltip
//
//  Мы обходим DrawConnection() полностью — он внутри зашит на Безье и
//  на флаг bTreatSplinesLikePins который может быть выключен.
// ─────────────────────────────────────────────────────────────────────────────

void FStraightNodesConnectionDrawingPolicy::DrawPolylineAndTest(
	const TArray<FVector2f>& Points, const FConnectionParams& Params)
{
	if (Points.Num() < 2) return;

	// Порог hover-а: берём из настроек движка как и оригинальный DrawConnection
	const float HoverThresholdSq = FMath::Square(Settings->SplineHoverTolerance + Params.WireThickness * 0.5f);

	bool bIntersectsSlice = false;

	for (int32 i = 0; i < Points.Num() - 1; ++i)
	{
		const FVector2f& P0 = Points[i];
		const FVector2f& P1 = Points[i + 1];

		// ── 1. Разрез Alt+ЛКМ ────────────────────────────────────────────────
		if (!bIntersectsSlice && SliceLine.IsValid())
		{
			if (SegmentsIntersect(P0, P1, SliceLine.StartPoint, SliceLine.EndPoint))
			{
				bIntersectsSlice = true;
				// Регистрируем пересечение — именно это и удаляет провод
				ConnectionsIntersectingSliceLine.Emplace(Params.AssociatedPin1, Params.AssociatedPin2);
			}
		}

		// ── 2. Hover / tooltip ───────────────────────────────────────────────
		{
			const FVector2f Closest = FMath::ClosestPointOnSegment2D(LocalMousePosition, P0, P1);
			const float DistSq = (LocalMousePosition - Closest).SizeSquared();

			if (DistSq < HoverThresholdSq)
			{
				if (DistSq < SplineOverlapResult.GetDistanceSquared())
				{
					const float DistToPin1 = (Params.AssociatedPin1 != nullptr) ? (P0 - Closest).SizeSquared() : FLT_MAX;
					const float DistToPin2 = (Params.AssociatedPin2 != nullptr) ? (P1 - Closest).SizeSquared() : FLT_MAX;
					SplineOverlapResult = FGraphSplineOverlapResult(
						Params.AssociatedPin1, Params.AssociatedPin2,
						DistSq, DistToPin1, DistToPin2, true);
				}
			}
		}
	}

	// ── 3. Рисуем полилайн ───────────────────────────────────────────────────
	FLinearColor WireColor = Params.WireColor;
	if (bIntersectsSlice)
	{
		WireColor.A *= SliceDeemphasisAlphaMultiplier;
	}

	FSlateDrawElement::MakeLines(
		DrawElementsList,
		WireLayerID,
		FPaintGeometry(),
		Points,
		ESlateDrawEffect::None,
		WireColor,
		true,
		Params.WireThickness);
}

// ─────────────────────────────────────────────────────────────────────────────
//  DrawSplineWithArrow overrides
// ─────────────────────────────────────────────────────────────────────────────

void FStraightNodesConnectionDrawingPolicy::DrawSplineWithArrow(
	const FVector2f& StartPoint, const FVector2f& EndPoint, const FConnectionParams& Params)
{
	TArray<FVector2f> Points;
	BuildRoutePoints(StartPoint, EndPoint, Points);

	DrawPolylineAndTest(Points, Params);

	// Bubbles для exec пинов — через базовый класс, направляем вдоль последнего сегмента
	if (Params.bDrawBubbles && Points.Num() >= 2)
	{
		const FVector2f LastDir  = (Points.Last() - Points[Points.Num() - 2]).GetSafeNormal();
		const FVector2f FakeCtrl = Points.Last() - LastDir * 0.01f;
		FKismetConnectionDrawingPolicy::DrawSplineWithArrow(FakeCtrl, EndPoint, Params);
	}
}

void FStraightNodesConnectionDrawingPolicy::DrawSplineWithArrow(
	const FGeometry& StartGeom, const FGeometry& EndGeom, const FConnectionParams& Params)
{
	const FVector2f Start = FVector2f(FGeometryHelper::VerticalMiddleRightOf(StartGeom));
	const FVector2f End   = FVector2f(FGeometryHelper::VerticalMiddleLeftOf(EndGeom));
	DrawSplineWithArrow(Start, End, Params);
}

void FStraightNodesConnectionDrawingPolicy::DrawPreviewConnector(
	const FGeometry& PinGeometry, const FVector2f& StartPoint, const FVector2f& EndPoint, UEdGraphPin* Pin)
{
	FConnectionParams Params;
	DetermineWiringStyle(Pin, nullptr, Params);
	DrawSplineWithArrow(StartPoint, EndPoint, Params);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Фабрика
// ─────────────────────────────────────────────────────────────────────────────

FConnectionDrawingPolicy* FStraightNodesConnectionDrawingPolicyFactory::CreateConnectionPolicy(
	const UEdGraphSchema* Schema, int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor,
	const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const
{
	if (Schema->IsA<UEdGraphSchema_K2>())
	{
		return new FStraightNodesConnectionDrawingPolicy(
			InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements, InGraphObj);
	}
	return nullptr;
}
