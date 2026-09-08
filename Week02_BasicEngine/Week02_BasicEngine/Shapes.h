// © 2024 KRAFTON, Inc. ALL RIGHTS RESERVED.

#pragma once
#include "TArray.h"
#include <cmath>
#include <d3d11.h>

struct FVertexSimple
{
	float x, y, z;    // Position
	float r, g, b, a; // Color
};

struct ResourceData
{
	TArray<FVertexSimple> Vertices;
	D3D11_PRIMITIVE_TOPOLOGY Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};

extern ResourceData LineResource;
extern ResourceData TriangleResource;
extern ResourceData PlaneResource;
extern ResourceData CubeResource;
extern ResourceData SphereResource;
extern ResourceData LocationGizmoResource;
extern ResourceData RotationGizmoResource;
extern ResourceData ScaleGizmoResource;

extern ResourceData FloorResource;

void CreateLocationGizmo(FVertexSimple* vertices, float r, float g, float b);

void CreateRotationGizmo(FVertexSimple* vertices, float r, float g, float b);

void InitializeVertexArray(TArray<FVertexSimple>& VertexArray, int32 ArrayCount);
