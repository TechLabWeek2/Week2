#include "UGizmo.h"

UGizmo::UGizmo(ETypeAxis axis)
{
	primitiveType = ETypePrimitive::Gizmo;
	Class = StaticClass();
	bIsActive = false;
	Type = ETypeTransform::Location;
	Axis = axis;
}

void UGizmo::Update(UPrimitiveComponent* Obj, FVector MouseMove)
{
    switch (Axis) {
    case ETypeAxis::XAxis:
        switch (Type) {
        case ETypeTransform::Location:
            Obj->RelativeLocation.x += MouseMove.x;
            break;
        case ETypeTransform::Rotation:
            Obj->RelativeRotation.x += MouseMove.x;
            break;
        case ETypeTransform::Scale:
            Obj->RelativeScale3D.x += MouseMove.x;
            break;
        }
        break;
    case ETypeAxis::YAxis:
        switch (Type) {
        case ETypeTransform::Location:
            Obj->RelativeLocation.y += MouseMove.y;
            break;
        case ETypeTransform::Rotation:
            Obj->RelativeRotation.y += MouseMove.y;
            break;
        case ETypeTransform::Scale:
            Obj->RelativeScale3D.y += MouseMove.y;
            break;
        }
        break;
    case ETypeAxis::ZAxis:
        switch (Type) {
        case ETypeTransform::Location:
            Obj->RelativeLocation.z += MouseMove.z;
            break;
        case ETypeTransform::Rotation:
            Obj->RelativeRotation.z += MouseMove.z;
            break;
        case ETypeTransform::Scale:
            Obj->RelativeScale3D.z += MouseMove.z;
            break;
        }
        break;
    }
}
