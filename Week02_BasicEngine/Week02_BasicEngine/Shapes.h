// © 2024 KRAFTON, Inc. ALL RIGHTS RESERVED.

#pragma once
#include "FMeshResource.h"

struct FVertexSimple
{
	float x, y, z;    // Position
	float r, g, b, a; // Color
};

enum ETypePrimitive {
	EPT_None,
	EPT_Triangle,
	EPT_Cube,
	EPT_Sphere,
	EPT_XLine,
	EPT_YLine,
	EPT_ZLine,
	EPT_Max,
};

extern FVertexSimple line_vertices[6];

extern FVertexSimple triangle_vertices[3];

extern FVertexSimple cube_vertices[36];

extern FVertexSimple sphere_vertices[2400];
