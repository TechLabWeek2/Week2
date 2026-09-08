// © 2024 KRAFTON, Inc. ALL RIGHTS RESERVED.

#pragma once
#include "TArray.h"
#include "FMeshResource.h"
#include <cmath>

struct FVertexSimple
{
	float x, y, z;    // Position
	float r, g, b, a; // Color
};
extern TArray<FVertexSimple> line_vertices;

extern TArray<FVertexSimple> triangle_vertices;

extern TArray<FVertexSimple> plane_vertices;

extern TArray<FVertexSimple> cube_vertices;
extern FVertexSimple floor1_vertices[12];
extern FVertexSimple floor2_vertices[12];

extern FVertexSimple cube_vertices[36];

extern TArray<FVertexSimple> sphere_vertices;
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
