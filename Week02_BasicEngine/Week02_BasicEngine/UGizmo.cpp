#include "UGizmo.h"

UGizmo::UGizmo()
{
	primitiveType = ETypePrimitive::Gizmo;
	Class = StaticClass();
	bIsActive = false;
	Type = UTypeTransform::Location;
}
