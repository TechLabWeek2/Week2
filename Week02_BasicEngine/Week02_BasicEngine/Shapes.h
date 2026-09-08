// © 2024 KRAFTON, Inc. ALL RIGHTS RESERVED.

#pragma once
#include "FMeshResource.h"

struct FVertexSimple
{
	float x, y, z;    // Position
	float r, g, b, a; // Color
};
extern FVertexSimple line_vertices[6];

extern FVertexSimple triangle_vertices[3];

extern FVertexSimple plane_vertices[12];

extern FVertexSimple cube_vertices[36];

extern FVertexSimple sphere_vertices[2400];
