// © 2024 KRAFTON, Inc. ALL RIGHTS RESERVED.

#pragma once
#include "FMeshResource.h"
#include <cmath>
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

extern FVertexSimple location_gizmo_x_vertices[144];
extern FVertexSimple location_gizmo_y_vertices[144];
extern FVertexSimple location_gizmo_z_vertices[144];

extern FVertexSimple scale_gizmo_x_vertices[93];
extern FVertexSimple scale_gizmo_y_vertices[93];
extern FVertexSimple scale_gizmo_z_vertices[93];

extern FVertexSimple rotation_gizmo_x_vertices[1536];
extern FVertexSimple rotation_gizmo_y_vertices[1536];
extern FVertexSimple rotation_gizmo_z_vertices[1536];

void CreateLocationGizmo(FVertexSimple* vertices, float r, float g, float b);

void CreateRotationGizmo(FVertexSimple* vertices, float r, float g, float b);