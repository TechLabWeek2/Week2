// © 2024 KRAFTON, Inc. ALL RIGHTS RESERVED.

#pragma once
//#include "FMeshResource.h"
#include "TArray.h"

struct FVertexSimple
{
	float x, y, z;    // Position
	float r, g, b, a; // Color
};
extern TArray<FVertexSimple> line_vertices;

extern TArray<FVertexSimple> triangle_vertices;

extern TArray<FVertexSimple> plane_vertices;

extern TArray<FVertexSimple> cube_vertices;

extern TArray<FVertexSimple> sphere_vertices;
